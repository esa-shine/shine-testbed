/*
 * server.cpp
 *
 *  Created on: 07 apr 2017
 *      Author: matteo
 */


//#include <string.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <unistd.h>
//#include <libssh/libssh.h>
//#include <libssh/sftp.h>
//#include <errno.h>
//#include <fcntl.h>
//#include <unistd.h>
//#include <stdio.h>
//#include <netdb.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <iostream>
//#include <fstream>
//#include <strings.h>
//#include <stdlib.h>
//#include <string>
//#include <pthread.h>
//#include <sstream>
//#include <iomanip>

/*****************includes for logic************/
#include "CheckFunction.h"
#include "EnvironmentSetup.h"
#include "DecodingInput.h"
#include "ConflictGraph.h"
#include "randomHandler.h"
#include "DataDefinition.h"
#include "grasp.h"
#include "hgcc.h"
#include "LibShine.h"

#include <queue>
#include <vector>

/** @file server.cpp
 *  \brief The main File to use the Server
 *    */

//using namespace std;

//#define HELLOPORT 12345
//#define HELLOGROUP "225.0.0.37"
#define MAXCONN 10
#define MAXCLIENT 10

std::vector<std::queue<int>*> queues;

// Thread functions
void *taskTCP(void * );
void *taskUDP(void * ); /**Thread to send Multicast*/
void * TaskTrasm(void *);

pthread_t threadA[MAXCLIENT];
pthread_t threadTaskUDP;

//vector <sockaddr_in> fd;

//struct sockaddr_in addr;

/**Number of trasmission x Size package;*/
//vector< vector<char> > coded_data;
//int max_size_package = NULL;

//header_transmission *header;
/**Variable to set the requested file*/
//int reqFile;
/**Shared struct*/
data_matrix data;
//int portNo, listenFd;
//int noThread;

//Setting id_udp_chunk variable
int id_udp_chunk = 0;

//cf_data outputForColoring;
int GRASP = 0;
//int *coloring = NULL;
/**Struct for loading configuration file .ini*/

Config config;
log4cpp::Appender *appender1;
log4cpp::Category& root= log4cpp::Category::getRoot();

/**main function */
int main(int argc, char* argv[]){/**\callgraph*/

	int portNo, listenFd;
	int noThread=0;

	loadConfigFile(config);

	appender1 = new log4cpp::RollingFileAppender("default", "program.log", config.maxSizeLog);
	appender1->setLayout(new log4cpp::BasicLayout());
	root.addAppender(appender1);

	//max_size_package = config.maxsizePackage;

	socklen_t len; //store size of the address
	int connFd;
	struct sockaddr_in svrAdd, clntAdd;
	if (argc < 2){
		root << log4cpp::Priority::ERROR  << "Syntax : ./Shine_srv <port>" ;
		cout << "Syntax: ./Shine_srv <port>" << endl; 
		return 0;
	}

	portNo = atoi(argv[1]);
	if((portNo > 65535) || (portNo < 2000)){
		root << log4cpp::Priority::ERROR   << "Please enter a port number between 2000 - 65535";
		cout << "Please enter a port number between 2000 - 65535" << endl;	
		return 0;
	}

	listenFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(listenFd < 0){
		root << log4cpp::Priority::ERROR  << "Cannot open socket" ;
		cout << "Cannot open socket" << endl;
		return 0;
	}

	bzero((char*) &svrAdd, sizeof(svrAdd));
	svrAdd.sin_family = AF_INET;
	svrAdd.sin_addr.s_addr = INADDR_ANY;
	svrAdd.sin_port = htons(portNo);

	/*bind socket */
	if(bind(listenFd, (struct sockaddr *)&svrAdd, sizeof(svrAdd)) < 0){
		root << log4cpp::Priority::ERROR << "Cannot bind";
		cout << "Cannot bind";
		return 0;
	}
	if(listen(listenFd, MAXCONN)<0){
		root << log4cpp::Priority::ERROR  <<"listen() failed.\n";
		cout << "listen() failed." << endl;
		close(listenFd);
		return 0;
	}

	vector <sockaddr_in> fd;

	while (noThread<config.numUtenti)
	{
		root << log4cpp::Priority::INFO  <<"noThread : "<<noThread;
		len = sizeof(clntAdd);
		root << log4cpp::Priority::INFO  << "Listening" ;
		/**this is where client connects. svr will hang in this mode until client conn*/
		connFd = accept(listenFd, (struct sockaddr *)&clntAdd, &len);

		fd.push_back(clntAdd);
		root << log4cpp::Priority::INFO <<"IP ADD client : "<<inet_ntoa(clntAdd.sin_addr);
		root << log4cpp::Priority::INFO <<"PORT client: " <<ntohs(clntAdd.sin_port);

		if (connFd < 0){
			root << log4cpp::Priority::ERROR << "Cannot accept connection" ;
			return 0;
		}
		else{
			cout << "Connection successful" << endl;
			root << log4cpp::Priority::INFO << "Connection successful";
		}

		std::queue<int> *queue = new std::queue<int>;
		queues.push_back(queue);

		thread_tcp_args* arg = (thread_tcp_args *)malloc(sizeof(*arg));
		arg->thread_connFd = connFd;
		arg->thread_noThread = noThread;

		pthread_create(&threadA[noThread], NULL,taskTCP, (void *)arg);
		//cin.ignore();
		noThread++;
/*
		int *arg= (int *)malloc(sizeof(*arg));
		*arg = connFd;
		pthread_create(&threadA[noThread], NULL,taskTCP, (void *)arg); //thread for tcp client read
		cin.ignore();
		noThread++;
*/
	}

	root << log4cpp::Priority::INFO << config.numUtenti << " users connected.....MAKING CACHE";
	cout<<"MAKING CACHE"<<endl;
	cout<< "noThread: " << noThread << endl;
	data=makeCache(fd,noThread,config.nFiles,config.maxsizePackage, config.pathRepo,config.per);
	cout<<"Cache popolate! "<<endl;
	root << log4cpp::Priority::INFO <<"Cache popolate! ";
	
	//OPENING UDP SOCKET

//	memset(&addr,0,sizeof(addr));

	/* create what looks like an ordinary UDP socket */
/*	if ((fdUDP=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
		root << log4cpp::Priority::ERROR << "socket";
		exit(1);
	}
*/
	/* set up destination address */
//	addr.sin_family=AF_INET;
//	addr.sin_addr.s_addr=inet_addr(config.hello_group);
//	addr.sin_port=htons(HELLOPORT);


	//Calling taskUDP every config.secondsMiss seconds.
	while(1){
		sleep(config.secondsMiss);
//		pthread_create(&threadA[noThread], NULL, taskUDP, NULL);
		pthread_create(&threadTaskUDP, NULL, taskUDP, NULL);
	
	}
}

/**TCP wait
 * Thread through which comes Miss*/
void *taskTCP (void * a){
	cout<<"task TCP "<<endl;
	thread_tcp_args *arg = (thread_tcp_args*)a;
	int connFd = arg->thread_connFd;
	int noThread = arg->thread_noThread;

	free(arg);

	string numeroT=to_string(noThread);
	if(write(connFd, numeroT.c_str(), sizeof(numeroT)) < 0){
		root << log4cpp::Priority::ERROR <<"errore nella write del noThread!";
		exit(0);
	}

	root << log4cpp::Priority::INFO<<"noThread inviato al client!!!";
	int utente=noThread;
	root << log4cpp::Priority::INFO<<"utente : "<<utente;
	root << log4cpp::Priority::INFO << "Thread per listen tcp created ";
	//char *test=new char[300];
	while(1){
		cout<<"wait on TCP... "<<endl;

		//char *test=new char[300];
		char *test = (char *) malloc(sizeof(char) * 300);

		int n = read(connFd, test, 255);
		if (n < 0){
			root << log4cpp::Priority::ERROR << " ERROR Lose Connection " ;
			break;
		}

		int reqFile;
		string tester (test);
		reqFile = atoi(test);
		cout << reqFile << endl;
		root << log4cpp::Priority::INFO <<"requested file : "<<reqFile;

		/** As soon as I get a miss, update the table-> for each miss i do this so
		 * when coding, the algorithm takes the miss as the required files
		 */

//		data.Q[utente]=reqFile;
cout<<"queues utente "<<endl;
		queues[utente]->push(reqFile);

		root << log4cpp::Priority::INFO<<"utente  : "<<utente;
		root << log4cpp::Priority::INFO<<"file rich : "<<reqFile;
		delete(test);
	}

	cout << "\nClosing thread and conn" << endl;
	close(connFd);
	return NULL;
}
/**UDP multicast send */
void *taskUDP(void * u){

	int n_col;

	//Fill "data" with the first elements of queues.

	for(int i=0; i<config.numUtenti; i++){
		data.Q[i] = NULL; //NULL -> 0
		if(!queues[i]->empty()){
			data.Q[i]=queues[i]->front();
			queues[i]->pop();
		}
	}

	root << log4cpp::Priority::INFO << "Conflict-Graph generator process-------------";
	cf_data outputForColoring = conflictGraphGenerator(data);
	root << log4cpp::Priority::INFO << "Numero nodi del grafo: " << outputForColoring.n_nodi;

	if (outputForColoring.n_nodi > 0){
		root << log4cpp::Priority::INFO << "Graph Coloring process-------------";
		/* Coloring*/

		/*TIME elapsed*/
		struct timeval start, end;
		long mtime, seconds, useconds;
		gettimeofday(&start, NULL);
		/**/	

		int *coloring;

		if (GRASP){
			int maxIter = 20;
			coloring = graspGraphColoring(maxIter, outputForColoring, &n_col);
			colorRienumeration(n_col, &coloring, outputForColoring.n_nodi);
		}else{
			coloring = hGCC(&n_col, data, outputForColoring);
			colorRienumeration(n_col, &coloring, outputForColoring.n_nodi);
		}

		root << log4cpp::Priority::INFO << "La colorazione e' stata effettuata con successo!";
		root << log4cpp::Priority::INFO << "Numero di colori utilizzati: " << n_col;

		gettimeofday(&end, NULL);

		seconds  = end.tv_sec  - start.tv_sec;
		useconds = end.tv_usec - start.tv_usec;

		mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
		cout<<"graph coloring elapsed time : "<<mtime<<endl;

		if (n_col > 0){
            root << log4cpp::Priority::INFO << "Coding data process-------------";

			thread_args *args = (thread_args *)malloc(sizeof(*args)); //FIXME: QUANDO FACCIO LA FREE?
			args->coloring_output = outputForColoring;
			args->num_colori = n_col;
			args->coloring_p = coloring;
//			args->coloring_data = dataForColoring;

//          cf_data *arg= (cf_data *)malloc(sizeof(*arg));
//          *arg = outputForColoring;

//          pthread_create(&threadA[noThread], NULL, TaskTrasm, (void *)arg); //thread per invio trasmissione UDP
//			pthread_create(&threadA[noThread], NULL, TaskTrasm, (void *)args); //thread per invio trasmissione UDP
			pthread_t threadTaskTrasm; 
			pthread_create(&threadTaskTrasm, NULL, TaskTrasm, (void *)args); //thread per invio trasmissione UDP
//			pthread_join(threadA[noThread],NULL);
			pthread_join(threadTaskTrasm,NULL);

			cout << "pthread terminated." << endl;
		}
	}
	
	cout<<endl<<"OK! wait for Miss"<<endl;
	return NULL;

	}

/**Thread for trasmitting.
 * In this thread are executed the algorithms to code and trasmitting data.*/
//void * TaskTrasm(void * a){

//	cf_data outputForColoring = *(a(cf_data *)a);

void * TaskTrasm(void * a){

	thread_args *args = (thread_args*)a;
	cf_data outputForColoring = args->coloring_output;
	int n_col = args->num_colori;
	int* coloring = args->coloring_p;
//	data_matrix dataForColoring = args->coloring_data;

	free(args);

	int fdUDP;
	
	/* create what looks like an ordinary UDP socket */
	if ((fdUDP=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
		root << log4cpp::Priority::ERROR << "socket";
		exit(1);
	}
	
	//Setting TTL
	int ttl = 64;
	setsockopt(fdUDP, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));


	struct sockaddr_in addr;
	memset(&addr,0,sizeof(addr));

	/* set up destination address */
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=inet_addr(config.hello_group);
	addr.sin_port=htons(config.hello_port);

	/*TIME elapsed*/
	struct timeval start, end;
	long mtime, seconds, useconds;
	gettimeofday(&start, NULL);
	/**/

	string pathFiles = config.pathRepo;
	nodo *nodi = outputForColoring.nodes;
	int n = outputForColoring.n_nodi;

	header_transmission *header = new header_transmission[n_col];
	if (!header){
		root << log4cpp::Priority::ERROR<< "MEMORY EXCEPTION: header memory allocation." ;
		exit(0);
	}
	char * copy=new char[config.maxsizePackage + 300];
	
	vector<string> files;
	files = getDirectoryFiles(pathFiles);
	if (files.empty()){
		root << log4cpp::Priority::ERROR << "Repository folder is empty or it not exist." ;
		exit(0);
	}

	/*Sort file by name*/
	sort(files.begin(), files.end());

	/*Find max size on each package*/

	vector< vector<char> > coded_data(n_col, vector<char>(config.maxsizePackage, 0));
	/* now just sendto() our destination! */

	/*For each transmission*/
	cout << "Numero di colori: " << n_col << endl;
	cout << "Numero di nodi: " << n << endl;



	for (int i = 0; i < n_col; i++){
		int padding=0;

		int id_utente;
		int id_file;
		int id_chunck;

		root << log4cpp::Priority::INFO<<"trasmissione numero -> "<<i;

			int color = i + 1;
			int begin_data = 1;

			int chunkForXor=0; /*how many chunck goes xoring*/

			/*For each node*/
			for (int j = 0; j < n; j++){

				if (coloring[j] == color){
					chunkForXor++;
					nodo *single_node = &(nodi)[j];

					id_utente =  single_node->id_utente;
					id_file =  single_node->id_file;
					id_chunck = single_node->id_chunck;
					int size_pkg;

					int trovato = 0;
					unsigned int q = 0;
					while (q < header[i].id_files.size() && !trovato){
						if (id_file == header[i].id_files.at(q) && id_chunck == header[i].id_chunks.at(q)){
							trovato = 1;
							size_pkg = header[i].size_package.at(q);
						}
						q++;
					}

					//cout << "Sending chunk " << id_chunck << " of file " << id_file << " to user " << id_utente << endl;
					//root << log4cpp::Priority::ERROR << "Sending chunk " << id_chunck << " of file " << id_file << " to user " << id_utente ;
							
					if (!trovato){
						/*Reading file */
						string file = files.at(id_file);
						ifstream is (file, ifstream::binary);

						if (is) {
							double size_file = getFileSize(file);
							int size_package = config.maxsizePackage;

							int begin_package = (id_chunck * size_package);
							if (begin_package > size_file){
								begin_package = size_file;
								size_package = 0;
							}else{
								if (begin_package + size_package > size_file){
									padding=1;	/*the last chunck*/
									size_package = abs(size_file - begin_package);
								}
							}

							if (size_package > 0){
								is.seekg(begin_package);

								char *buffer = new char[size_package];

								is.read(buffer, size_package);
								is.close();
								for (int k = 0; k < size_package; k++){
									if (begin_data){
										coded_data.at(i).at(k) = buffer[k];
									}else{
										coded_data.at(i).at(k) = (char) coded_data.at(i).at(k) ^ buffer[k];
										chunkForXor++;
									}

								}
								delete[] buffer;
								begin_data = 0;
							}
							header[i].id_utenti.push_back(id_utente);
							header[i].id_files.push_back(id_file);
							header[i].id_chunks.push_back(id_chunck);
							header[i].size_package.push_back(size_package);
						}else{
							root << log4cpp::Priority::ERROR << "Error opening file in storage: " << file ;
							exit(0);
						}
					}else{
						header[i].id_utenti.push_back(id_utente);
						header[i].id_files.push_back(id_file);
						header[i].id_chunks.push_back(id_chunck);
						header[i].size_package.push_back(size_pkg);
					}
				}
				
			}

				/*Now i know how many chunks are associated to the same color*/
			//cout << "chunkForXor = " << chunkForXor << endl;

			if(chunkForXor==1){	/* if it is only one*/
				int indiceChunk =0; //CHANGED FROM 1 TO 0
				/*i have to find another chunk for xored */
				while(data.Ind[id_utente][id_file][indiceChunk]==0){
					indiceChunk++;
				}
				string file = files.at(id_file);
				ifstream is (file, ifstream::binary);
				if (is) {
					double size_file = getFileSize(file);
					int size_package = config.maxsizePackage;
					int begin_package = (indiceChunk * size_package);
					if (begin_package > size_file){
						begin_package = size_file;
						size_package = 0;
					}else{
						if (begin_package + size_package > size_file){
							size_package = abs(size_file - begin_package);
						}
					}
					if (size_package > 0){
						is.seekg(begin_package);
						char *buffer = new char[size_package];
						is.read(buffer, size_package);
						is.close();
						for (int k = 0; k < size_package; k++){
							if (begin_data){
								coded_data.at(i).at(k) = buffer[k];
								root << log4cpp::Priority::ERROR <<"NOT GOOD ";
							}else{
								/*xoring*/
								coded_data.at(i).at(k) = (char) coded_data.at(i).at(k) ^ buffer[k];
							}

						}
						delete[] buffer;
						begin_data = 0;
					}else{
						root << log4cpp::Priority::ERROR << "Error opening file in storage: " << file ;
						exit(0);
					}
					header[i].id_utenti.push_back(-INF);
					header[i].id_files.push_back(id_file);
					header[i].id_chunks.push_back(indiceChunk);
					header[i].size_package.push_back(size_package);
				}
			}
			

			header[i].padding=padding;
			header[i].num_chunks=header[i].id_chunks.size();
				/*---------------------------------SEND PHASE--------------------------------*/

			int size=0;
			char intToString [32];
			//char * copy=new char[config.maxsizePackage + 300];
			int sizeCoded=coded_data[i].size();
			root << log4cpp::Priority::INFO <<"size coded "<<coded_data[i].size();


			/*----------------------------------HEADER----------------------------------*/

			/*			PADDING				*/
			root << log4cpp::Priority::INFO<<"padding "<< padding;
			sprintf(intToString,"%d",padding);
			strcpy(copy,intToString);
			strcat(copy," ");
			size++;
			size++;

			/*			NUM CHUNKS			*/
			int sizeHchunk = header[i].num_chunks;
			root << log4cpp::Priority::INFO<<"sizeHchunk "<< sizeHchunk;
			sprintf(intToString,"%d",sizeHchunk);
			strcat(copy,intToString);
			strcat(copy," ");
			size++;
			size++;

			/*			ID UTENTI			*/
			for(unsigned int h=0;h<header[i].id_utenti.size();h++){
				root << log4cpp::Priority::INFO<<"4.id_utenti : "<< header[i].id_utenti[h];
				sprintf(intToString,"%d",header[i].id_utenti[h]);
				strcat(copy,intToString);
				strcat(copy," ");
				if(header[i].id_utenti[h]==0){
					size++;
				}else{
					for(;header[i].id_utenti[h];header[i].id_utenti[h]/=10){
						size++;
					}
				}

				size++;
			}

			/*			ID FILES 			*/
			for(unsigned int h=0;h<header[i].id_files.size();h++){
				root << log4cpp::Priority::INFO<<"2.id_files "<< header[i].id_files[h];
				sprintf(intToString,"%d",header[i].id_files[h]);
				strcat(copy,intToString);
				strcat(copy," ");
				if(header[i].id_files[h]==0){
					size++;
				}else{
					for(;header[i].id_files[h];header[i].id_files[h]/=10){
						size++;
					}
				}
				size++;
			}

			/*			ID CHUNKS			*/
			for(unsigned int h=0;h<header[i].id_chunks.size();h++){
				root << log4cpp::Priority::INFO<<"1.head_chunks "<< header[i].id_chunks[h];
				sprintf(intToString,"%d",header[i].id_chunks[h]);
				strcat(copy,intToString);
				strcat(copy," ");
				if(header[i].id_chunks[h]==0){
					size++;
				}else{
					for(;header[i].id_chunks[h];header[i].id_chunks[h]/=10){
						size++;
					}
				}
				size++;
			}



			/*			SIZE PACKAGE			*/
			for(unsigned int h=0;h<header[i].size_package.size();h++){
				root << log4cpp::Priority::INFO<<"3.size p "<< header[i].size_package[h];
				sprintf(intToString,"%d",header[i].size_package[h]);
				strcat(copy,intToString);
				strcat(copy," ");
				for(;header[i].size_package[h];header[i].size_package[h]/=10){
					size++;
				}
				size++;
			}



			/*			TOTAL SIZE  (HEADER+PAYLOAD)			*/
			int j=size;
			size=size+sizeCoded;
			int dim = size;
			for(;dim;dim/=10){
				size++;
				j++;
			}
			size++;
			j++;
			size++;
			j++;//ADDED FABIO
			root << log4cpp::Priority::INFO<<"size tot : "<< size;
			sprintf(intToString,"%d",size);
			strcat(copy,intToString);
			strcat(copy," ");

			/*			CODED DATA			*/
			//char * prova=new char[sizeCoded];
			memcpy(&copy[j],coded_data[i].data(),sizeCoded);
//			memcpy(prova,coded_data[i].data(),sizeCoded);
//			memcpy(&copy[j],prova,sizeCoded);
			//delete[] prova;

			/*			SEND			*/
//			if (sendto(fdUDP,copy,size,0,(struct sockaddr *) &addr, sizeof(addr)) < 0) {
//				root << log4cpp::Priority::ERROR <<"sendto";
//				exit(1);
//			}
			int maxSizeFragment = config.maxSizeFragment;
			if (send_fragmented_udp(fdUDP,copy,size,0,(struct sockaddr *) &addr, sizeof(addr),maxSizeFragment,id_udp_chunk) < 0) {
				root << log4cpp::Priority::ERROR <<"sendto";
				exit(1);
			}
			else{
				id_udp_chunk++;
			}
			//delete[] copy;

		/*	aggiorno la matrice */
		usleep(100); //ADDED FABIO

//		cout << "data.Ind[id_utente][id_file][id_chunck]: " << "data.Ind[" << id_utente << "][" << id_file << "][" << id_chunck << "] da 0 diventa 1 " << data.Ind[id_utente][id_file][id_chunck] << endl;
//		if(!id_file)
//			data.Ind[id_utente][id_file][id_chunck]=1;
		//	usleep(100);

	}

	delete[] header;

	delete[] copy;
	//free(outputForColoring.Matrix_Adj);
	//free(nodi);

	close(fdUDP);

	free(coloring);

	//DEALLOCAZIONE MATRIX_ADJ, NODES E TUTTO CIÒ CHE È STATO ALLOCATO E NON SERVE PIÙ

	gettimeofday(&end, NULL);

	seconds  = end.tv_sec  - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;

	mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
	cout<<"taskTrasm elapsed time : "<<mtime<<endl;

	root << log4cpp::Priority::INFO<<"TERMINATED: ";
	cout<<endl<<"TERMINATED: "<<endl;
	return NULL;
}
