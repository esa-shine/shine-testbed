/*
 * LibShine.h
 *
 *  Created on: 24 mag 2017
 *      Author: matteo
 */

#ifndef LIBSHINE_H_
#define LIBSHINE_H_

#include <log4cpp/Category.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/RollingFileAppender.hh>

#include <sys/time.h>
#include <vector>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include <string>
#include <math.h>
#include <arpa/inet.h>
#include <fstream>
#include <algorithm>
#include <sstream>

#include "EnvironmentSetup.h"
#include "CheckFunction.h"
#include "randomHandler.h"
#include "DecodingInput.h"
#include "utilityForTesting.h"

using namespace std;

#define SECONDSATTESA 1
//#define NFILES 5
//#define CHUNKS 200
//#define MAXSIZEPACKAGE 1000

int Attesa(int );

data_matrix makeCache(vector <sockaddr_in>, int, int, int, string,int);

int verify_knownhost(ssh_session session);

void colorRienumeration(int , int **, int );

int* makeVectorChuncks(int m_files, int, string);
struct Config {
	int hello_port;//=12345;
	char* hello_group= new char[10];//="255.0.0.37"
	int nFiles;//=5
	int secondsMiss;//=5
	int secondsStop;//=1
	int maxsizePackage;//=1000
	string pathRepo;//="./repository"
	int maxClient;
	int maxSizeLog;
	int numUtenti;
	int per;
	int maxSizeFragment;
};
void loadConfigFile(Config &);
//log4cpp::Appender *  makeLog();

ssize_t send_fragmented_udp(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen, size_t MAX_PACKET_SIZE, int id_udp_chunk);

#endif /* LIBSHINE_H_ */
