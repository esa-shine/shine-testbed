#!/bin/bash

#ldconfig

#in usr/lib

cd usr/lib

# FOR EXECUTION

rm -f liblog4cpp.so.5
ln -s liblog4cpp.so.5.0.6 liblog4cpp.so.5

rm -f libperl.so.5.18
ln -s libperl.so.5.18.2 libperl.so.5.18  

rm -f libgslcblas.so.0
ln -s libgslcblas.so.0.0.0 libgslcblas.so.0

rm -f libgsl.so.0
ln -s libgsl.so.0.10.0 libgsl.so.0

# FOR COMPILATION

rm -f liblog4cpp.so
ln -s liblog4cpp.so.5.0.6 liblog4cpp.so

rm -f libgsl.so
ln -s libgsl.so.0.10.0 libgsl.so

rm -f libgslcblas.so
ln -s libgslcblas.so.0.0.0 libgslcblas.so

cd ../../
