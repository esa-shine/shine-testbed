/*
 * LibShine.cpp
 *
 *  Created on: 21 giu 2017
 *      Author: matteo
 */
/** @file LibShine.cpp
 *  \brief The file to load configuration file
 *    */
#include "LibShine.h"



void loadConfigFile(Config & config){
    ifstream fin("configFile.ini");
    string line;

    while (getline(fin, line)) {
        istringstream sin(line.substr(line.find("=") + 1));
        if (line.find("hello_port") != -1){
            sin >> config.hello_port;
//            cout<<"ciao"<<config.hello_port<<endl;
        }
        else if (line.find("hello_group") != -1){
            sin >> config.hello_group;
//            cout<<"ciao"<<config.hello_group<<endl;
        }
        else if (line.find("nFiles") != -1){
            sin >> config.nFiles;
//            cout<<"ciao"<<config.nFiles<<endl;
        }
        else if (line.find("maxSizePackage") != -1){
        	sin >> config.maxSizePackage;
//        	cout<<"ciao"<<config.secondsMiss<<endl;
        }
//        else if (line.find("secondsStop") != -1){
//            sin >> config.secondsStop;
////            cout<<"ciao"<<config.secondsStop<<endl;
//        }
//        else if (line.find("maxsizePackage") != -1){
//            sin >> config.maxsizePackage;
////            cout<<"ciao"<<config.maxsizePackage<<endl;
//        }
        else if (line.find("pathRepo") != -1){
            sin >> config.pathRepo;
//            cout<<"ciao"<<config.pathRepo<<endl;
        }
        else if (line.find("maxSizeFragment") != -1){
        	sin >> config.maxSizeFragment;
//        	cout<<"ciao"<<config.secondsMiss<<endl;
        }
    }

}

int countDigits(int num){
    //The code above is made of a series of if statements because it must be fast		
    if (num < 0) return 0;
    else if (num < 10) return 1;
    else if (num < 100) return 2;
    else if (num < 1000) return 3;
    else if (num < 10000) return 4;
    else if (num < 100000) return 5;
    else if (num < 1000000) return 6;
    else if (num < 10000000) return 7;
    else if (num < 100000000) return 8;
    else if (num < 1000000000) return 9;
    else return 10; //If you need more of 10 digits for size, add other if statements
}