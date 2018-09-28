/*
 * LibShine.cpp
 *
 *  Created on: 24 mag 2017
 *      Author: matteo
 */
/** @file LibShine.cpp
 *  \brief A file to execute the main functions
 *    */
#include "LibShine.h"

/**function to count the chunks for each file*/
int* makeVectorChuncks(int m_files, int maxSizePackage, string pathFiles){

	log4cpp::Category& sub1 = log4cpp::Category::getInstance(std::string("makeVectorChuncks"));
	cout<<"num chunks"<<endl;
	int * num_chunks= new int[m_files];

	vector<string> files;
	files = getDirectoryFiles(pathFiles);

	if (files.empty()){
		sub1 << log4cpp::Priority::ERROR  << "Repository folder is empty or it not exist." ;
		exit(0);
	}

	    /*Sort file by name*/
	sort(files.begin(), files.end());

	    /*Find max size on each package*/
	int i=0;
	for (string f : files){
		int size_file = getFileSize(f);
		num_chunks[i]=ceil(size_file/(double)maxSizePackage);
		i++;
	}
	return num_chunks;
}

int Attesa(int secondsAttesa){
	sleep(secondsAttesa);
	return 0;
}
/**function to preload the caches*/
data_matrix makeCache(vector <sockaddr_in> fd, int noThread, int nFiles, int maxSizePackage, string pathFiles, int percentage){

	int numero_utenti = noThread;
	int m_files = nFiles;
	cout<<"b_chunks"<<endl;
	int *b_chuncks = new int[nFiles];

	b_chuncks=makeVectorChuncks(m_files, maxSizePackage, pathFiles);

	log4cpp::Category& sub3 = log4cpp::Category::getInstance(std::string("makeCache"));

	int i,j;

	data_matrix data;


	double **probs = (double **) malloc (numero_utenti * sizeof(double *));
	//double *probs[numero_utenti];
	check_memory_double_allocation_2D(probs, "Allocation 2D Probs.");
	for (i=0; i<numero_utenti; i++){
		cout<<"probs array"<<endl;
		probs[i] = new double[m_files] ;
		check_memory_double_allocation_1D(probs[i], "Allocation 1D Probs.");
	}

	/*********************CHIEDERE ALLA PROF TULINO PER LE PROBS*********************/

	for (i=0; i<numero_utenti; i++){
		for (j=0; j<m_files; j++){
			probs[i][j] = randomNumber(0, 1);
		}
	}

	int **M = (int **) malloc (numero_utenti * sizeof(int *));
	check_memory_allocation_2D(M, "Allocation 2D M.");
	for (i=0; i<numero_utenti; i++){
		M[i] = (int *) malloc (m_files * sizeof(int));
		check_memory_allocation_1D(M[i], "Allocation 1D M.");
	}

	/*************************CUBO PER ASSEGNAZIONE CHUNK************************/
	//dovrebbe essere relativo a quello che già si ha nella cache
	//lo faccio la prima volta per popolare le cache
	float per;
		if(percentage==0){
			per = 0;
		}else{
			per = 100/(float)percentage;
		}	
		cout << "Percentace choosen: " << percentage << ", per = " << per << endl;
		for (i=0; i<numero_utenti; i++){
			for (j=0; j<m_files; j++){
				if(per==0){
					M[i][j]=0;
				}else{
					M[i][j] = ceil((float)b_chuncks[j]/per);//diviso 5 ad esempio gli do il 20% di chunk per ogni file //round(randomNumber(0, b_chuncks[j])); //#chunk
					cout << "User " << i << " have " << M[i][j] << " chunks of file " << j <<", that have " << b_chuncks[j] << " chunks." << endl;
					//cout << "b_chuncks[j]/per = " << b_chuncks[j]/per << endl;
					//cout << "ceil(b_chuncks[j]/per) = " << ceil(b_chuncks[j]/per) << endl; 
				}
			}
		}


	int access_type = O_WRONLY | O_CREAT | O_TRUNC;
	/* Create a Demand and Caches in a random way */
	vector<int> input = setEnvironment(numero_utenti, m_files, b_chuncks, probs, M);
	if (input.size() > 0){
		cout << endl << "Decoding process" << endl << "-------------" << endl << endl;
		sub3 << log4cpp::Priority::INFO  << "Decoding process";

		data = fromArrayToMatrix(m_files, b_chuncks, input); //ha tutte le info su probs e M

	    for (int k = 0; k < data.n_utenti; k++){
	    	int id_utente = k;
	        int id_requested_file =  data.Q[id_utente];
	        sub3 << log4cpp::Priority::INFO<<"req File "<< id_requested_file;
	        char * check = inet_ntoa(fd[k].sin_addr);
	        sub3 << log4cpp::Priority::INFO<<"IP utente: "<<check;

	        /*************************************************************/

	        ssh_session my_ssh_session;
	        int rc;
	        // Open session and set options
	        my_ssh_session = ssh_new();
	        if (my_ssh_session == NULL)
	        	exit(-1);
	        ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, check);

			//int sshport = 5022;

			//ssh_options_set(my_ssh_session, SSH_OPTIONS_PORT, &sshport);
			
	        // Connect to server
	        rc = ssh_connect(my_ssh_session);
	        if (rc != SSH_OK)
	        {
	        	sub3 << log4cpp::Priority::ERROR << "Error connecting to localhost:"<<ssh_get_error(my_ssh_session);
	        	ssh_free(my_ssh_session);
	        	exit(-1);
	        }
	        // Verify the server's identity
	        // For the source code of verify_knowhost(), check previous example
	        if (verify_knownhost(my_ssh_session) < 0)
	        {
	        	ssh_disconnect(my_ssh_session);
	        	ssh_free(my_ssh_session);
	        	exit(-1);
	        }

	        rc = ssh_userauth_publickey_auto(my_ssh_session, NULL,"");
	        if (rc == SSH_AUTH_ERROR)
	        {
	        	sub3 << log4cpp::Priority::ERROR << "Authentication failed::"<<ssh_get_error(my_ssh_session);
	        }
	        sub3 << log4cpp::Priority::INFO<<"ssh attached";

	        sftp_session sftp;
	        sftp = sftp_new(my_ssh_session);
	        if (sftp == NULL)
	        {
	        	sub3 << log4cpp::Priority::ERROR << "Error allocating SFTP session:"<<ssh_get_error(my_ssh_session);
	        }
	        rc = sftp_init(sftp);
	        if (rc != SSH_OK)
	        {
	        	sub3 << log4cpp::Priority::ERROR << "Error initializing SFTP session:"<<sftp_get_error(sftp);
	        	sftp_free(sftp);
	        }
	        //cout<<"STOQUA"<<endl;

	        rc = sftp_mkdir(sftp, "cache", S_IRWXU);
	        if (rc != SSH_OK)
	        {
	        	if (sftp_get_error(sftp) != SSH_FX_FILE_ALREADY_EXISTS)
	        	{
	        		sub3 << log4cpp::Priority::ERROR << "Can't create directory:"<<ssh_get_error(my_ssh_session);
	        	}
	        }

/**********************************************************/

	        sub3 << log4cpp::Priority::INFO <<  "User id: " << id_utente;
	        sub3 << log4cpp::Priority::INFO<< "Request File id: " << id_requested_file ;

	        vector<string> files;
	        files = getDirectoryFiles(pathFiles);

	        if (files.empty()){
	        	sub3 << log4cpp::Priority::ERROR << "Repository folder is empty or it not exist." ;
	            exit(0);
	        }

	          /*Sort file by name*/
	        sort(files.begin(), files.end());
	        sub3 << log4cpp::Priority::INFO << "Request File Name: " << files.at(id_requested_file);
	        for (int i = 0; i < m_files; i++){
			cout << "Size of " << to_string(i) << ": " << to_string(b_chuncks[i]) << endl;
	        	for (int j = 0; j < b_chuncks[i]; j++){
//				cout << "data.Ind[id_utente][i][j]: " << "data.Ind[" << id_utente << "][" << i << "][" << j << "] = " << data.Ind[id_utente][i][j] << endl;
	        		if ((data.Ind[id_utente][i][j] == 1) ){

					//Creation of file '/home/cache/sizeof_i_b_chuncks[i]_.info'
/*
                         char *buffer_size = new char[1]();
                         string save_size = "/home/cache/sizeof_"+to_string(i)+"_"+to_string(b_chuncks[i])+"_.info";
                         const char* file_save_size = save_size.c_str();
                         sftp_file file_size = sftp_open(sftp,file_save_size,access_type,S_IRWXU);
					if (file_size == NULL)
					{
						sub3 << log4cpp::Priority::ERROR << "Can't open file for writing:"<<ssh_get_error(my_ssh_session);
						fprintf(stderr, "Can't open file for writing: %s\n",ssh_get_error(my_ssh_session));
					}

                         int nwritten_size = sftp_write(file_size, buffer_size, 1);
					if (nwritten_size != 1)
					{
						sub3 << log4cpp::Priority::ERROR << "Can't write data to file: "<<ssh_get_error(my_ssh_session);
						fprintf(stderr, "Can't write data to file: %s\n",ssh_get_error(my_ssh_session));
						sftp_close(file_size);
					}
                         rc=sftp_close(file_size);
					if (rc != SSH_OK)
					{
						sub3 << log4cpp::Priority::ERROR << "Can't close the written file: "<<ssh_get_error(my_ssh_session);
						fprintf(stderr, "Can't close the written file: %s\n",ssh_get_error(my_ssh_session));
					}
                         delete[] buffer_size;
*/

	        			/*Reading files*/
	        			string file = files.at(i);
						ifstream is (file, ifstream::binary);
						if (is) {
							double size_file = getFileSize(file);
							int size_package = maxSizePackage;
							int begin_package = (j * size_package);
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
								sftp_file file;
								string save = "/home/cache/_"+to_string(i)+"_"+to_string(j)+".cache";
								const char * file_save = save.c_str();
								int  nwritten;
								file = sftp_open(sftp, file_save,access_type, S_IRWXU);
								if (file == NULL)
								{
									sub3 << log4cpp::Priority::ERROR << "Can't open file for writing:"<<ssh_get_error(my_ssh_session);
									fprintf(stderr, "Can't open file for writing: %s\n",ssh_get_error(my_ssh_session));
								}
								nwritten = sftp_write(file, buffer, size_package);
								if (nwritten != size_package)
								{
									sub3 << log4cpp::Priority::ERROR << "Can't write data to file: "<<ssh_get_error(my_ssh_session);
									fprintf(stderr, "Can't write data to file: %s\n",ssh_get_error(my_ssh_session));
									sftp_close(file);
								}
								rc = sftp_close(file);
								if (rc != SSH_OK)
								{
									sub3 << log4cpp::Priority::ERROR << "Can't close the written file: "<<ssh_get_error(my_ssh_session);
									fprintf(stderr, "Can't close the written file: %s\n",ssh_get_error(my_ssh_session));
								}
								delete[] buffer;
							}
							is.close();
						}else{
							cout << endl << "Error opening file in storage: " << file << endl;
							exit(0);
						}
	        		}
	        	}
	        }
	        sftp_free(sftp);
	        ssh_disconnect(my_ssh_session);
	        ssh_free(my_ssh_session);
	    }
	}
	return data;
}

int verify_knownhost(ssh_session session)
{
  int state, hlen;
  unsigned char *hash = NULL;
  char *hexa;
  char buf[10];
  state = ssh_is_server_known(session);
  hlen = ssh_get_pubkey_hash(session, &hash);
  if (hlen < 0)
    return -1;
  switch (state)
  {
    case SSH_SERVER_KNOWN_OK:
      break; /* ok */
    case SSH_SERVER_KNOWN_CHANGED:
      fprintf(stderr, "Host key for server changed: it is now:\n");
      ssh_print_hexa("Public key hash", hash, hlen);
      fprintf(stderr, "For security reasons, connection will be stopped\n");
      free(hash);
      return -1;
    case SSH_SERVER_FOUND_OTHER:
      fprintf(stderr, "The host key for this server was not found but an other"
        "type of key exists.\n");
      fprintf(stderr, "An attacker might change the default server key to"
        "confuse your client into thinking the key does not exist\n");
      free(hash);
      return -1;
    case SSH_SERVER_FILE_NOT_FOUND:
      fprintf(stderr, "Could not find known host file.\n");
      fprintf(stderr, "If you accept the host key here, the file will be"
       "automatically created.\n");
      /* fallback to SSH_SERVER_NOT_KNOWN behavior */
    case SSH_SERVER_NOT_KNOWN:
      hexa = ssh_get_hexa(hash, hlen);
      fprintf(stderr,"The server is unknown. Do you trust the host key?\n");
      fprintf(stderr, "Public key hash: %s\n", hexa);
      free(hexa);
    //   if (fgets(buf, sizeof(buf), stdin) == NULL)
    //   {
    //     free(hash);
    //     return -1;
    //   }
    //   if (strncasecmp(buf, "yes", 3) != 0)
    //   {
    //     free(hash);
    //     return -1;
    //   }
      if (ssh_write_knownhost(session) < 0)
      {
        fprintf(stderr, "Error %s\n", strerror(errno));
        free(hash);
        return -1;
      }
      break;
    case SSH_SERVER_ERROR:
      fprintf(stderr, "Error %s", ssh_get_error(session));
      free(hash);
      return -1;
  }
  free(hash);
  return 0;
}

void colorRienumeration(int n_col, int **coloring, int n){
    int new_col = 0;
    for (int i = 0; i < n; i++){
        int col = i + 1;
        int isThere = 0;
        for (int j = 0; j < n; j++){
            if ((*coloring)[j] == col){
                (*coloring)[j] = new_col + 1;
                isThere = 1;
            }
        }

        if (isThere){
            new_col++;
        }
    }

    if (new_col != n_col){
        cout << endl << "Number color error: new: " << new_col << " original: " << n_col << endl;
        exit(0);
    }
}
/**function to load configuration file*/
void loadConfigFile(Config & config){
    ifstream fin("configFile.ini");
    string line;

    while (getline(fin, line)) {
        istringstream sin(line.substr(line.find("=") + 1));
        if (line.find("hello_port") != -1){
            sin >> config.hello_port;
        }
        else if (line.find("hello_group") != -1){
            sin >> config.hello_group;
        }
        else if (line.find("nFiles") != -1){
            sin >> config.nFiles;
        }
        else if (line.find("secondsMiss") != -1){
        	sin >> config.secondsMiss;
        }
        else if (line.find("secondsStop") != -1){
            sin >> config.secondsStop;
        }
        else if (line.find("maxsizePackage") != -1){
            sin >> config.maxsizePackage;
        }
        else if (line.find("pathRepo") != -1){
            sin >> config.pathRepo;
        }
        else if (line.find("maxSizeLog") != -1){
        	sin >> config.maxSizeLog;
        }
        else if (line.find("numUtenti") != -1){
        	sin >> config.numUtenti;
        }
        else if (line.find("per") != -1){
        	sin >> config.per;
        }
        else if (line.find("maxSizeFragment") != -1){
        	sin >> config.maxSizeFragment;
        }
    }

}

//FUNCTION FOR UDP FRAGMENTED PACKETS SENDING

ssize_t send_fragmented_udp(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen,size_t maxSizeFragment, int id_udp_chunk){
	unsigned char* ptr = (unsigned char*) buf;

	int id_fragment = 0;
	int tot_fragments = (int)(len / maxSizeFragment) + 1;

//	cout << "len = " << len << ", maxSizeFragment = " << maxSizeFragment << endl; 

	size_t total = 0;
	size_t newsize = 0;
	char intToString[32];

	while(total < len){

		//cout << "SENDING FRAGMENT " << id_fragment << " , WHICH RESIDES ON " << ptr << " BYTES. total = " << total << endl;

		newsize = len - total; 
		if(newsize > maxSizeFragment){
			newsize = maxSizeFragment;
		}

		int size = 0;
		char * copy = new char[maxSizeFragment + 1024];
		
		//Adding id_chunk
		sprintf(intToString,"%d",id_udp_chunk);
		strcpy(copy,intToString);
		strcat(copy," ");
		size += strlen(intToString) + 1;

		//Adding NumTotFrags
		sprintf(intToString,"%d",tot_fragments);
		strcat(copy,intToString);
		strcat(copy, " ");
		size += strlen(intToString) + 1;

		//Adding id_fragment
		sprintf(intToString,"%d",id_fragment);
		strcat(copy,intToString);
		strcat(copy," ");
		size += strlen(intToString) + 1;

//		cout << "FRAGMENT HEADER = " << copy << endl;

		//Adding fragment
		//strncat(copy, (const char *) ptr, maxSizeFragment);
		memcpy(copy + size, (const char *) ptr, maxSizeFragment);
		size += maxSizeFragment;

		
//		if(id_fragment == 0){
//			cout << "copy = " << copy << "\nSize of copy = " << size << endl;
//		}

		size_t result = sendto(sockfd, copy, size, flags, dest_addr, addrlen);
		if(result<0){
			return -1;
		}
		else{
			delete[] copy;
			total += maxSizeFragment;
			ptr += maxSizeFragment;
			id_fragment++;
		}

	}
//	delete[] buffer;
	return (ssize_t)total;
}
