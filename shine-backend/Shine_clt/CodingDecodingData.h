#ifndef CODINGDECODINGDATA_H_INCLUDED
#define CODINGDECODINGDATA_H_INCLUDED

//#define MAXSIZEPACKAGE 64000

#include <log4cpp/Category.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/RollingFileAppender.hh>
#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <fstream>
#include <algorithm>
#include "utilityForTesting.h"
#include "DataDefinition.h"

using namespace std;

//vector< vector<char> > codingData(int *coloring, int n_col, data_matrix data, cf_data outputForColoring, header_transmission **header_data);
void decodingData(header_transmission header, vector<char> &coded_data, int m_files,  int id_utente, int id_requested_file, string, int);

#endif // CODINGDECODINGDATA_H_INCLUDED
