#!/bin/bash

cd metricbeat.client
./run
sleep 1
cd ..
cd packetbeat.client
./run
sleep 1
cd ..
