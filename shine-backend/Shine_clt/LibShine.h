/*
 * LibShine.h
 *
 *  Created on: 21 giu 2017
 *      Author: matteo
 */
#include <string>
#include <sstream>
#include <stdlib.h>
#include <fstream>

using namespace std;

#ifndef LIBSHINE_H_
#define LIBSHINE_H_

struct Config {
	int hello_port;//=12345;
	char* hello_group= new char[10];//="255.0.0.37"
	int nFiles;//=5
	string pathRepo;//="./repository"
	int maxSizePackage;
	int maxSizeFragment;
};

void loadConfigFile(Config &);

//When fragmented chunks arrives
typedef struct FRAGMENTED_CHUNK{
	int id_chunk;
	int num_fragments;
	int count;
	char* chunk;
}Fragmented_chunk;

typedef struct THREAD_SAVE_ARGS{
	char* fragment;
}thread_save_args;


int countDigits(int num);

#endif /* LIBSHINE_H_ */
