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

/** @file server.cpp
 *  \brief The main File to use the Server
 *    */

//using namespace std;

#define HELLOPORT 12345
#define HELLOGROUP "225.0.0.37"
#define MAXCONN 10
#define MAXCLIENT 10


void *taskTCP(void * );
/**Thread to send Multicast*/
void *taskUdp(void * );

void * TaskTrasm(void *);
vector <sockaddr_in> fd ;
/**Number of trasmission x Size package;*/
vector< vector<char> > coded_data;
header_transmission *header;
/**Variable to set the requested file*/
int reqFile;
/**Shared struct*/
data_matrix data;
pthread_t threadA[MAXCLIENT];
int pId, portNo, listenFd;
int noThread;
int numero_utenti=noThread;
bool primoCM=false;
int trasmissioneAttiva=0;
cf_data outputForColoring;
int GRASP = 1;
int *coloring = NULL;
int n_col;
/**Struct for loading configuration file .ini*/
Config config;
log4cpp::Appender *appender1;
log4cpp::Category& root= log4cpp::Category::getRoot();

/**main function */
int main(int argc, char* argv[])

{/**\callgraph*/
	loadConfigFile(config);
	appender1 = new log4cpp::RollingFileAppender("default", "program.log", config.maxSizeLog);
	appender1->setLayout(new log4cpp::BasicLayout());
	root.addAppender(appender1);


    socklen_t len; //store size of the address
    int connFd;
    struct sockaddr_in svrAdd, clntAdd;
    if (argc < 2)
    {
    	root << log4cpp::Priority::ERROR  << "Syntam : ./server <port>" ;
        return 0;
    }
    portNo = atoi(argv[1]);
    if((portNo > 65535) || (portNo < 2000))
    {
    	root << log4cpp::Priority::ERROR   << "Please enter a port number between 2000 - 65535";
        return 0;
    }

    listenFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(listenFd < 0)
    {
    	root << log4cpp::Priority::ERROR  << "Cannot open socket" ;
        return 0;
    }

    bzero((char*) &svrAdd, sizeof(svrAdd));
    svrAdd.sin_family = AF_INET;
    svrAdd.sin_addr.s_addr = INADDR_ANY;
    svrAdd.sin_port = htons(portNo);

    /*bind socket */
    if(bind(listenFd, (struct sockaddr *)&svrAdd, sizeof(svrAdd)) < 0)
    {
    	root << log4cpp::Priority::ERROR   << "Cannot bind" ;
        return 0;
    }
    if(listen(listenFd, MAXCONN)<0){
    	root << log4cpp::Priority::ERROR  <<"listen() failed.\n";
    	close(listenFd);
    	return 0;
    }
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

        if (connFd < 0)
        {
        	root << log4cpp::Priority::ERROR << "Cannot accept connection" ;
        	return 0;
        }
        else
        {
        	cout << "Connection successful" << endl;
        	root << log4cpp::Priority::INFO << "Connection successful";
        }
        int *arg= (int *)malloc(sizeof(*arg));
        *arg = connFd;
        pthread_create(&threadA[noThread], NULL,taskTCP, (void *)arg); //thread for tcp client read
        cin.ignore();
        noThread++;
    }
    root << log4cpp::Priority::INFO <<"4 users connect.....MAKING CACHE";
    cout<<"MAKING CACHE"<<endl;
    data=makeCache(fd,noThread,config.nFiles,config.maxsizePackage, config.pathRepo,config.per);
    cout<<"Cache popolate! "<<endl;
    root << log4cpp::Priority::INFO <<"Cache popolate! ";
    while(1){
    }
}

/**TCP wait
 * Thread through which comes Miss*/
void *taskTCP (void * a)
{
	int connFd = *((int *)a);
	string numeroT=to_string(noThread);
	if(write(connFd, numeroT.c_str(), sizeof(numeroT)) < 0){
		root << log4cpp::Priority::ERROR <<"errore nella write del noThread!";
		exit(0);
	}
	root << log4cpp::Priority::INFO<<"noThread inviato al client!!!";
	int utente=noThread;
	root << log4cpp::Priority::INFO<<"utente : "<<utente;
	root << log4cpp::Priority::INFO << "Thread per listen tcp created ";
    char *test=new char[300];
    while(1)
    {
    	cout<<"wait on TCP... "<<endl;

    	int n = read(connFd, test, 255);
    	if (n < 0) {
    		root << log4cpp::Priority::ERROR << " ERROR Lose Connection " ;
    		break;
    	}

    	string tester (test);
    	reqFile=atoi(test);
    	cout << reqFile << endl;
    	root << log4cpp::Priority::INFO <<"requested file : "<<reqFile;



    	/** As soon as I get a miss, update the table-> for each miss i do this so
    	 * when coding, the algorithm takes the miss as the required files
    	 */

    	data.Q[utente]=reqFile;

    	root << log4cpp::Priority::INFO<<"utente  : "<<utente;
    	root << log4cpp::Priority::INFO<<"file rich : "<<reqFile;


    	if(!primoCM){
    		primoCM = true;
    		pthread_create(&threadA[noThread], NULL, taskUdp, NULL);
    	}else if(trasmissioneAttiva==1){
    		trasmissioneAttiva=Attesa(config.secondsStop);
    	}
    }
    cout << "\nClosing thread and conn" << endl;
    close(connFd);
    return NULL;
}
/**UDP multicast send */
void *taskUdp(void * u){

	/**wait for other miss for a fixed time*/
	sleep(config.secondsMiss);

	primoCM=false;

	root << log4cpp::Priority::INFO << "Conflict-Graph generator process-------------";
	outputForColoring = conflictGraphGenerator(data);
	root << log4cpp::Priority::INFO << "Numero nodi del grafo: " << outputForColoring.n_nodi ;

	if (outputForColoring.n_nodi > 0){
		root << log4cpp::Priority::INFO << "Graph Coloring process-------------";
	    /* Coloring*/
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

	    if (n_col > 0){
	    	root << log4cpp::Priority::INFO << "Coding data process-------------";

	    	pthread_create(&threadA[noThread], NULL, TaskTrasm, NULL); //thread per invio trasmissione UDP

	    }
	}
	cout<<endl<<"OK! wait for Miss"<<endl;

	return NULL;
}
/**Thread for trasmitting.
 * In this thread are executed the algorithms to code and trasmitting data.*/
void * TaskTrasm(void *){

	/*TIME elapsed*/
		 struct timeval start, end;

		    long mtime, seconds, useconds;

		    gettimeofday(&start, NULL);

		/**/

	trasmissioneAttiva=1;
	string pathFiles = config.pathRepo;
	nodo *nodi = outputForColoring.nodes;
	int n = outputForColoring.n_nodi;

	header_transmission *header = new header_transmission[n_col];
	if (!header){
		root << log4cpp::Priority::ERROR<< "MEMORY EXCEPTION: header memory allocation." ;
		exit(0);
	}

	vector<string> files;
	files = getDirectoryFiles(pathFiles);
	if (files.empty()){
		root << log4cpp::Priority::ERROR << "Repository folder is empty or it not exist." ;
		exit(0);
	}

	/*Sort file by name*/
	sort(files.begin(), files.end());

	/*Find max size on each package*/
	double max_size = -INF;
	for (string f : files){
		int size_file = getFileSize(f);

		if (size_file > max_size){
			max_size = size_file;
		}
	}
	int max_size_package = config.maxsizePackage;

	vector< vector<char> > coded_data(n_col, vector<char>(max_size_package, 0));
	int id_utente ;
	int id_file ;
	int id_chunck;
	int padding;
	struct sockaddr_in addr;
	int fd;


	memset(&addr,0,sizeof(addr));

	/* create what looks like an ordinary UDP socket */
	if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
		root << log4cpp::Priority::ERROR << "socket";
		exit(1);
	}

	/* set up destination address */
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=inet_addr(config.hello_group);
	addr.sin_port=htons(HELLOPORT);

	/* now just sendto() our destination! */

    /*For each transmission*/
	for (int i = 0; i < n_col; i++){
		padding=0;
		root << log4cpp::Priority::INFO<<"trasmissione numero -> "<<i;
		if(trasmissioneAttiva==1){
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

			if(chunkForXor==1){	/* if it is only one*/
				int indiceChunk =1;
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
			char * copy=new char[config.maxsizePackage + 100];
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
			root << log4cpp::Priority::INFO<<"size tot : "<< size;
			sprintf(intToString,"%d",size);
			strcat(copy,intToString);
			strcat(copy," ");

			/*			CODED DATA			*/
			char * prova=new char[sizeCoded];
			memcpy(prova,coded_data[i].data(),sizeCoded);
			memcpy(&copy[j],prova,sizeCoded);

			/*			SEND			*/
			if (sendto(fd,copy,size,0,(struct sockaddr *) &addr, sizeof(addr)) < 0) {
				root << log4cpp::Priority::ERROR <<"sendto";
				exit(1);
			}
			delete(copy);
		}

		/*	aggiorno la matrice */
		data.Ind[id_utente][id_file][id_chunck]=1;

	}
	gettimeofday(&end, NULL);

	seconds  = end.tv_sec  - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;

	mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
	cout<<"elapsed time : "<<mtime<<endl;

	root << log4cpp::Priority::INFO<<"TERMINATED: ";
	cout<<endl<<"TERMINATED: "<<endl;
	return NULL;
}
