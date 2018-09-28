#!/bin/bash

tar -zxvf log4cpp-1.1.2.tar.gz
cd log4cpp
./configure
make
make install
