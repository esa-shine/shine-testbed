#!/bin/bash
cd elasticsearch
./build
cd ..
cd wazuh-manager
./build
cd ..
cd kibana
./build
cd ..
cd logstash
./build
cd ..
cd metricbeat
./build
cd ..
cd packetbeat
./build
cd ..
# cd centos_container
# ./build
# cd ..
# cd ubuntu_container
# ./build
# cd ..
