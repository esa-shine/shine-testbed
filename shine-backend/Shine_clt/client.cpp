/*
 * client.cpp
 *
 *  Created on: 07 apr 2017
 *      Author: matteo
 */

/** @file client.cpp
 *  \brief The main file to use the client
 *    */
#include "CodingDecodingData.h"
#include "LibShine.h"


//#include <string.h>
//#include <time.h>
//#include <algorithm>
//#include <stdio.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <iostream>
//#include <fstream>
//#include <sstream>
//#include <iomanip>
//#include <strings.h>
//#include <stdlib.h>
//#include <string>
//#include <time.h>q
//#include <vector>
//#include <pthread.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>

using namespace std;

void *taskUdp(void * );
void *taskSave(void * );

//#define HELLO_PORT 12345
//#define HELLO_GROUP "225.0.0.37"
#define MSGBUFSIZE 1024

#define MAXCONN 10

#include <sys/poll.h>
#include <sys/ioctl.h>
#define TRUE 1
#define FALSE 0


pthread_t threadA[3];
int id_requested_file = -INF; //Maybe to eliminate
int id_utente=0;

//For UDP framentation
std::vector<Fragmented_chunk*> fragments_vector;

//Other variables
int connFd;
int finished = 0;

//Global variables because they never change!
//int sizeCoded=64000;
//int size=65535;
int sizeCoded;
int size;

//Queue for packet receiving
#include<queue>
std::queue<char*> received_fragments;

/**Struct for loading configuration file .ini*/
Config config;
log4cpp::Appender *appender1;
log4cpp::Category& root= log4cpp::Category::getRoot();

/**main function
 * this function open a TCP connection to the server and run a thread to Listen UDP  */
int main (int argc, char* argv[])
{
	loadConfigFile(config);
	appender1 = new log4cpp::RollingFileAppender("default", "program.log",config.maxSizePackage);
	appender1->setLayout(new log4cpp::BasicLayout());
	root.addAppender(appender1);

	int listenFd, portNo;
	struct sockaddr_in svrAdd;
	struct hostent *server;

	sizeCoded = config.maxSizePackage;
	size = sizeCoded + (sizeCoded/10);

	printf("NEW CLIENT!!");

	if(argc < 3){
		root << log4cpp::Priority::ERROR <<"Syntax: ./Shine_clt <host name> <port>";
		cout << "Syntax: ./Shine_clt <host name> <port>" << endl;
		return 0;
	}

	portNo = atoi(argv[2]);
	if((portNo > 65535) || (portNo < 2000)){
		root << log4cpp::Priority::ERROR <<"Please enter port number between 2000 - 65535";
		cout << "Please enter port number between 2000 - 65535" << endl;
		return 0;
	}

	if(pthread_create(&threadA[0],NULL,taskUdp,NULL)!=0){
		root << log4cpp::Priority::ERROR <<"Error creating taskUDP thread";
		cout << "Error creating taskUDP thread" << endl;
	}

	if(pthread_create(&threadA[1],NULL,taskSave,NULL)!=0){
		root << log4cpp::Priority::ERROR <<"Error creating taskUDP thread";
		cout << "Error creating taskUDP thread" << endl;
	}


	listenFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(listenFd < 0){
		root << log4cpp::Priority::ERROR <<"Cannot open socket";
		cout << "Cannot open socket" << endl;
		return 0;
	}

	server = gethostbyname(argv[1]);
	
	if(server == NULL){
		root << log4cpp::Priority::ERROR << "Host does not exist";
		cout << "Host does not exist" << endl;
		return 0;
	}

	bzero((char *) &svrAdd, sizeof(svrAdd));
	svrAdd.sin_family = AF_INET;
	bcopy((char *) server -> h_addr, (char *) &svrAdd.sin_addr.s_addr, server -> h_length);
	svrAdd.sin_port = htons(portNo);
	int checker = connect(listenFd,(struct sockaddr *) &svrAdd, sizeof(svrAdd));

	if (checker < 0){
		root << log4cpp::Priority::ERROR <<"Cannot connect!";
		cout << "Cannot connect!" << endl;
		return 0;
	}

	char *numeroT = new char[10];
	int n = read(listenFd,numeroT, 5);
	if (n < 0) {
		root << log4cpp::Priority::ERROR << " ERROR Lose Connection ";
		cout << "ERROR: Connection lost." << endl;
		exit(0);
	}

	id_utente=atoi(numeroT);
	root << log4cpp::Priority::INFO<<"My user id is: "<<id_utente;

	//Initialization for socket between NodeJS and Client-NC
	struct sockaddr_in srvBackend;
	int serverFd;
	int connFd;
	int reqFile;
	
	serverFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    	if(serverFd < 0){
		root << log4cpp::Priority::ERROR  << "Cannot open socket" ;
		cout << "ERROR Cannot open socket." << endl;
		return 0;
	}
	
	bzero((char*) &srvBackend, sizeof(srvBackend));
	srvBackend.sin_family = AF_INET;
	srvBackend.sin_addr.s_addr = INADDR_ANY;
	srvBackend.sin_port = htons(8080);

	if(bind(serverFd, (struct sockaddr *)&srvBackend, sizeof(srvBackend)) < 0){
		root << log4cpp::Priority::ERROR   << "Cannot bind" ;
		cout << "Cannot bind" << endl;
		return 0;
	}

	if(listen(serverFd, MAXCONN)<0){
		root << log4cpp::Priority::ERROR  <<"listen() failed.\n";
		cout << "listen() failed." << endl;
		close(serverFd);
		return 0;
	}


	//Connection with storage-dash-nodeJS accepted only one time
	socklen_t len = sizeof(srvBackend);
	
	connFd = accept(serverFd, (struct sockaddr *)&srvBackend, &len);
	if (connFd < 0){
		root << log4cpp::Priority::ERROR << "Cannot accept connection" ;
		cout << "Cannot accept connection" << endl;
		return 0;
	}

	for(;;){
		char *test=new char[255]();

//		cout << "\nInsert id of requested file: " << endl;
//		cin >> test;
	    	int n = read(connFd, test, 255);
	    	if (n < 0) {
	    		root << log4cpp::Priority::ERROR << " ERROR Lose Connection " ;
			cout << "ERROR Lose Connection" << endl;
	    		break;
	    	}
//		gettimeofday(&start, NULL);

		std::string reqFile(test);
	    	cout << "REQUESTED FILE:" << reqFile.c_str() << endl;
	    	root << log4cpp::Priority::INFO <<"requested file : "<<reqFile;

		write(listenFd, reqFile.c_str(), sizeof(reqFile));
		
		string response = "File requested: " + reqFile;
		if(write(connFd, response.c_str(), sizeof(response)) < 0){
			root << log4cpp::Priority::ERROR <<"errore nella write della response!";
		exit(0);
		}

		/*
		if(write(connFd, reqFile.c_str(), sizeof(reqFile)) < 0){
			root << log4cpp::Priority::ERROR <<"errore nella write della response!";
		exit(0);
		}
		*/
//		id_requested_file=atoi(reqFile.c_str());
//		cout << "ID_REQUESTED_FILE: " << id_requested_file << endl;

		delete(test);
	}

/*
	for(;;){
		char *s=new char[2];
		cout << "Enter stuff: "<<endl;
		cin >> s;
		write(listenFd, s, strlen(s));
		id_requested_file=atoi(s);
	}
*/
}

/**Thread for recive data.
 * This thread open a UDP connection for listen UDP and are executed the algorithms to decode and save data.*/
void *taskUdp(void * prt){
	string source_iface;
	string group(config.hello_group);
	int port(config.hello_port);

	int fd;
	int rc = 1;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		root << log4cpp::Priority::ERROR<< "socket";
	    exit(-1);
	}

	//Allow socket descriptor to be reuseable
	u_int yes = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
	{
		root << log4cpp::Priority::ERROR<<"Reusing ADDR failed";
		exit(-1);
	}

	//Set socket to be nonblocking.
	if (ioctl(fd, FIONBIO, (char *)&yes) < 0){
		perror("ioctl() failed");
		exit(-1);
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = (source_iface.empty() ? htonl(INADDR_ANY) : inet_addr(source_iface.c_str()));
	if (bind(fd,(struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		root << log4cpp::Priority::ERROR<<"bind";
		exit(1);
	}

	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr(group.c_str());
	mreq.imr_interface.s_addr = (source_iface.empty() ? htonl(INADDR_ANY) : inet_addr(source_iface.c_str()));

	if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
	{
		cout << strerror(errno);
		root << log4cpp::Priority::ERROR<<"setsockopt";
		exit(1);
	}

	socklen_t addrlen;
	int nbytes;
	addrlen = sizeof(addr);

	int end_server = FALSE;
	int close_conn;

	//Initialize the pollfd structure
	struct pollfd fds[200];
	int nfds = 1;
	memset(fds, 0, sizeof(fds));
	fds[0].fd = fd;
	fds[0].events = POLLIN;

	//Initialize the timeout to 10 minutes.
	int timeout = (9000 * 60 * 1000);

	int compress_array = FALSE;

	do{
		//Call poll and wait 3 minutes for it to complete
		printf("Waiting on poll()...\n");
		rc = poll(fds, nfds, timeout);

		//Check if the poll call failed
		if(rc < 0){
			perror("poll() failed");
			break;
		}
		
		//Check to see if the 3 minute time out expired
		if (rc == 0){
			printf(" poll() timed out. End program.\n");
			break;
		}

		//One or more descriptors are readable. Need to determine which ones they are.
		int current_size = nfds;
		int i = 0;
		for (i = 0; i < current_size; i++){
			//Loop through to find the descriptors that returned POLLIN and determine whether it's the listening
			//or the active connection
			if(fds[i].revents == 0)
				continue;
			
			//If revents is not POLLN, it's an unexpected result, log and end the server
			if(fds[i].revents != POLLIN){
				printf(" error! revents = %d\n", fds[i].revents);
				end_server = TRUE;
				break;
			}

			//UDP protocol: no listen!
			printf("Descriptor %d is readable\n", fds[i].fd);
			close_conn = FALSE;
			//Receive all incoming data on this socket
			//before we loop back and call poll again.

			do{
				//Receive data on this connection until the recv
				//fails with EWOULDBLOCK. If any other failure occurs,
				//we will close the connection
				
				char * fragment;
				int fragment_size = config.maxSizeFragment + 1024;
				try {
					fragment = new char [fragment_size];
					//fragment = (char*)malloc(fragment_size);
				} catch (std::bad_alloc&) {
					root << log4cpp::Priority::ERROR <<"Not enough memory to create fragment.";
					throw;
				}

				rc = recv(fds[i].fd, fragment, fragment_size, 0);
				if(rc < 0){
					if(errno != EWOULDBLOCK){
						perror ("recv() failed");
						close_conn = TRUE;
					}
					break;
				}

				if (rc == 0){
					printf("Connection closed\n");
					close_conn = TRUE;
					break;
				}

				//Data was received
				printf("%d bytes received\n", rc);
				received_fragments.push(fragment);

			}while(TRUE);
		if(close_conn){
			close(fds[i].fd);
			fds[i].fd = -1;
			compress_array = TRUE;
		}
	} //CLOSING FOR

	if(compress_array){
		compress_array = FALSE;
		for (i = 0; i < nfds; i++){
			if (fds[i].fd == -1){
				for (int j = 1; j < nfds; j++){
					fds[j].fd = fds[j+1].fd;
				}
				i--;
				nfds--;
			}
		}
	}
} while(end_server == FALSE); //End of server running
/*
	while (1)
	{
		root << log4cpp::Priority::INFO <<"UDP wait on recive";

		char * fragment;
		int fragment_size = config.maxSizeFragment + 1024;
		try {
			fragment = new char [fragment_size];
			//fragment = (char*)malloc(fragment_size);
		} catch (std::bad_alloc&) {
			root << log4cpp::Priority::ERROR <<"Not enough memory to create fragment.";
			throw;
		}

		if ((nbytes = recvfrom(fd,fragment, fragment_size, 0, (struct sockaddr *)&addr, &addrlen)) < 0)
			{
				root << log4cpp::Priority::ERROR <<"recvfrom";
				perror("recvfrom");
				exit(1);
			}
			received_fragments.push(fragment);
		}
*/	
}


void *taskSave(void *a){
	while(1){
		while(received_fragments.empty()){
			usleep(1);
		}
	
			char* fragment = received_fragments.front();
			received_fragments.pop();

			int ptr = 0;
			int id_chunk_fragment = -INF;
			char* temp;

			int maxSizeFragment = config.maxSizeFragment;
			int fragment_size = maxSizeFragment + 1024;

			//Obtain the id_chunk from the fragment
			temp= (char*)malloc(fragment_size);
			strcpy(temp,&fragment[ptr]);
			id_chunk_fragment = atoi(temp);
			ptr += countDigits(id_chunk_fragment)+1;
			free(temp);

			//Check if the chunk exists
			int found = 0;
			int k = 0;

			while(k < fragments_vector.size() && found == 0){
				if(id_chunk_fragment == fragments_vector[k]->id_chunk){
					found = 1;
				} else{
					k++;
				}
			}
			
			cout << "k value = " << k << endl;

			if(found == 0){
				//CREATE ENTRY IN VECTOR
				Fragmented_chunk* fragmented_chunk = new Fragmented_chunk();
				fragmented_chunk->id_chunk = id_chunk_fragment;

				//Obtain num fragments
				temp= (char*)malloc(fragment_size);
				strcpy(temp,&fragment[ptr]);
				int total_fragments = atoi(temp);
				//cout << "Total number of fragments = " << total_fragments << endl;
				ptr += countDigits(total_fragments)+1;
				free(temp);

				fragmented_chunk->num_fragments = total_fragments;

				//Obtain id_fragment
				temp= (char*)malloc(fragment_size);
				strcpy(temp,&fragment[ptr]);
				int id_fragment = atoi(temp);
				//cout << "ID of fragment = " << id_fragment << endl;
				ptr += countDigits(id_fragment)+1;
				free(temp);
				
				//Allocate new memory area for chunk
				fragmented_chunk->chunk = (char*)malloc(size);

				//Copy fragment in proper position
				//cout << "Fragment inserted at position" << id_fragment*maxSizeFragment << endl;
				memcpy(&fragmented_chunk->chunk[id_fragment*maxSizeFragment],&fragment[ptr],maxSizeFragment);

				//Initializing fragment count
				fragmented_chunk->count = 1;

				//Saving chunk in vector
				fragments_vector.insert(fragments_vector.begin()+k,fragmented_chunk);
				delete[] fragment;

			} else{
				//UPDATE ENTRY IN VECTOR
				//Obtain and skip num of fragments
				temp= (char*)malloc(fragment_size);
				strcpy(temp,&fragment[ptr]);
				int total_fragments = atoi(temp);
				ptr += countDigits(total_fragments)+1;
				free(temp);

				//Obtain id_fragment
				temp= (char*)malloc(fragment_size);
				strcpy(temp,&fragment[ptr]);
				int id_fragment = atoi(temp);
				//cout << "ID of fragment = " << id_fragment << endl;
				ptr += countDigits(id_fragment)+1;
				free(temp);

				//Copy fragment in proper position
				memcpy(&fragments_vector[k]->chunk[id_fragment*maxSizeFragment],&fragment[ptr],maxSizeFragment);
				delete[] fragment;

				//Update fragment count
				fragments_vector[k]->count++;
			}

			//Controllo se il chunk "k" è stato completato.
			if(fragments_vector[k]->count == fragments_vector[k]->num_fragments){
				char* s = new char [size];
				
				cout << "Chunk " << fragments_vector[k]->id_chunk << " completely received!" << endl;

				memcpy(s,fragments_vector[k]->chunk,size);

				header_transmission * header=new header_transmission();

				char * coded;
				try {
					coded = new char [sizeCoded+1];
				} catch (std::bad_alloc&) {
					root << log4cpp::Priority::ERROR <<"Not enough memory to create size.";
					throw;
				}

				int hc, hf, hp , hu;
				int padding=0;
				int sizeHchunck=0;
				int sizeTot;
				int i=0;
				int id_chunk = 0;

					/*					PADDING				*/
				char * a= (char*)malloc(size);
				strcpy(a,&s[i]);
				padding = atoi(a);
				header->padding=padding;
				root << log4cpp::Priority::INFO<<"padding : "<<padding;
				delete(a);
				i++;
				i++;


					/*					NUM CHUNKS				*/
				a= (char*)malloc(size);
				strcpy(a,&s[i]);
				sizeHchunck = atoi(a);
				header->num_chunks=sizeHchunck;
				root << log4cpp::Priority::INFO<<"sizeHchunck : "<<sizeHchunck;
				delete(a);
				i++;
				i++;

				int index = -INF;
				int id_file = -INF;

					/*					ID UTENTI				*/
				for (int h=0;h<sizeHchunck;h++){

					char * a= (char*)malloc(size);
					strcpy(a,&s[i]);
					hu=atoi(a);
					header->id_utenti.push_back(hu);
					root << log4cpp::Priority::INFO<<"4 utenti : "<<hu;

					if(hu == id_utente){
						index = h;
						//cout << "This chunk is for me! index = " << index << endl;
					}


					if(hu==0){
						i++;
					}else{
						for(;hu;hu/=10){
							i++;
						}
					}
					delete(a);
					i++;
				}

					/*					ID FILES				*/
				for (int h=0;h<sizeHchunck;h++){
					char * a= (char*)malloc(size);
					strcpy(a,&s[i]);
					hf = atoi(a);
					if(h == index){
						id_file = hf;
						//cout << "At index " << index << " there is file " << id_file << endl;
					}
					root << log4cpp::Priority::INFO<<"2 files : "<<hf;
					header->id_files.push_back(hf);
					if(hf==0){
						i++;
					}else{
						for(;hf;hf/=10){
							i++;
						}
					}
					delete(a);
					i++;
				}

					/*					ID CHUNKS				*/
				for (int h=0;h<sizeHchunck;h++){
					char * a= (char*)malloc(size);
					strcpy(a,&s[i]);
					hc = atoi (a);
					if(h == index){
						id_chunk = hc;
					}
					header->id_chunks.push_back(hc);
					root << log4cpp::Priority::INFO<<"1 chunks : "<<hc;
					if(hc==0){
						i++;
					}else{
						for(;hc;hc/=10){
							i++;
						}
					}
					delete(a);
					i++;
				}


					/*					SIZE PACKAGE				*/
				for (int h=0;h<sizeHchunck;h++){
					char * a= (char*)malloc(size);
					strcpy(a,&s[i]);
					hp = atoi(a);
					root << log4cpp::Priority::INFO<<"3 size pack : "<<hp;
					header->size_package.push_back(hp);
					for(;hp;hp/=10){
						i++;
					}
					delete(a);
					i++;
				}

					/*					SIZE TOT				*/

				//BUG FIXING CODE: We need to know number of digits of total size to perform next operations
				/*
				* WARNING: There could be troubles if you choose a sizeCoded value too close to
				* a number with a different number of digits (for example, 999 for 3 digits, 9998 for
				* 4 digits, and so on). It is recommended to choose a safety limit of 50 below the
				* value with a different number of digits. For example, for 5 digits is better to choose
				* a value less than 99950).
				*/

				int total_size_temp = sizeCoded + i;
				int numDigits = countDigits(total_size_temp);

				//Snippet of original code
				a= (char*)malloc(size);
				strcpy(a,&s[i]);

				//Snippet of fixing code
				char* buffer_temp = (char*)malloc(numDigits + 1);
				for (int t=0; t<numDigits + 1 ; t++){
					buffer_temp[t] = a[t];
				}
		//		cout << "[LOG] BUFFER TEMP = " << buffer_temp << endl; 
				sizeTot = atoi(buffer_temp);
				delete(buffer_temp);

				//Original code
		//		sizeTot = atoi(a);
				header->sizeTot=sizeTot;
				root << log4cpp::Priority::INFO<<"size Totale : "<<sizeTot;
				int dim = sizeTot;
				delete(a);
				for(;dim;dim/=10){
					i++;
				}
				i++;
				i++;

				vector<char> v(&s[i], &s[i] + (sizeCoded+1));

				//If a chunk of file 0 arrived, the pre-fetching has finished.
				if(finished == 0){		
					if(id_file == 0){
						//Sending pre-fetching finished signal to NodeJS
						string finish = "finish";
		//				cout << "STRING " << finish.c_str() << ", size: " << sizeof(finish)-1 << endl;
						if(write(connFd, finish.c_str(), sizeof(finish)-1) < 0){
							root << log4cpp::Priority::ERROR <<"errore nella write della finish!";
							exit(0);
						}
						finished = 1;
					}
				}

				//cout << "ID FILE:" << id_file << endl;
				
				if(id_file != -INF){
					decodingData(*header, v , config.nFiles, id_utente, id_file, config.pathRepo, config.maxSizePackage);
				}else{
					//cout << "Chunk not for me..." << endl;
				}

				root << log4cpp::Priority::INFO<<"chunck received";

		//		cout<<endl<<"Chunk " << id_chunk << " of file " << id_file << " received "<<endl;
		/*
				//Send to NodeJS id of chunk and id of file

				string toSend = "_" + to_string(id_chunk) + "_" + to_string(id_file) + "_";
				cout << "Sending: " << toSend << endl;
				if(write(connFd, toSend.c_str(), sizeof(toSend)-1) < 0){
					root << log4cpp::Priority::ERROR <<"errore nella write della toSend!";
					exit(0);
				}
		*/

				//free(fragments_vector[k]->chunk);
				
				delete(s);
				delete(coded);
				delete(header);

	//			delete(fragments_vector[k]->chunk);
				cout << "Erasing element " << k << endl;
				free(fragments_vector[k]->chunk);
				fragments_vector.erase(fragments_vector.begin()+k);
	//			cout << "Dimension of fragments_vector after erase: " << fragments_vector.size() << endl;
				//fragments_vector.erase(&fragments_vector[k]);
		}
	}
}
