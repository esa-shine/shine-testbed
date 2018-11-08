#!/bin/bash

cd elasticsearch
./run
cd ..
until nc -z 192.168.200.130 9200;
do
        echo "ElasticSearch not running... Wait" 
        sleep 5
done

cd wazuh-manager
./run
cd ..
until nc -z 192.168.200.135 55000;
do
        echo "WazuhManager API not running... Wait" 
        sleep 5
done

cd kibana
./run
until nc -z 192.168.200.131 5601;
do
        echo "Kibana not running... Wait" 
        sleep 5
done
./kibana_import.sh
cd ..
cd wazuh-manager
./add_SHINE_properties.sh
cd ..

cd logstash
./run
cd ..
until nc -z 192.168.200.129 5044;
do
        echo "LogStash not running... Wait" 
        sleep 5
done

cd metricbeat
./run
sleep 1
cd ..
cd packetbeat
./run
sleep 1
cd ..
# cd centos_container
# ./run
# cd ..
# cd ubuntu_container
# ./run
# cd ..
