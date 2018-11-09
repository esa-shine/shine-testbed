#!/usr/bin/env bash
# git clone git://git.libwebsockets.org/libwebsockets $DEPS_HOME/dl/libwebsockets

git clone https://libwebsockets.org/repo/libwebsockets $DEPS_HOME/dl/libwebsockets
cd $DEPS_HOME/dl/libwebsockets
mkdir build
cd build
cmake -DLWS_MAX_SMP=1 -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_C_FLAGS="-fpic" ..
make && make install
