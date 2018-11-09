#!/bin/bash

cd metricbeat
./run
sleep 1
cd ..
cd packetbeat
./run.client
sleep 1
cd ..

