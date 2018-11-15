#!/bin/bash

BOLD='\033[1;32m'
COLOR='\033[0;32m'
NC='\033[0m'

BACKEND_PORT_BASE=8100

IPNCServer=192.168.19.142
# IPNCServer="$(docker inspect -f '{{ .NetworkSettings.Networks.shineNCnetwork.IPAddress }}' NC-dash-server)"

# echo -e "${COLOR}\n>> Now the shineNCnetwork overlay network will be created.${NC}"
# docker network create shineNCnetwork
rm -rf /home/opensand/Desktop/shine-testbed/ConfigFile/clt 
rm -rf /home/opensand/Desktop/shine-testbed/mpd
rm -rf /home/opensand/Desktop/shine-testbed/mlst
cp -R /home/opensand/Desktop/clt /home/opensand/Desktop/shine-testbed/ConfigFile/clt
cp -R  /home/opensand/Desktop/mpd /home/opensand/Desktop/shine-testbed/mpd
cp -R /home/opensand/Desktop/mlst /home/opensand/Desktop/shine-testbed/mlst
cp -R /home/opensand/Desktop/id_rsa.pub /home/opensand/Desktop/shine-testbed/
echo -e "${COLOR}\n>> Running shine storage"

docker run -d --name shine-storage -p 8686:8686 -p 8687:8687 -p 9601:9601 -p 8690:8690 -p 8688:8688 --mount type=bind,source=/home/opensand/Desktop/shine-testbed/mpd,target=/usr/src/app/mpd --mount type=bind,source=/home/opensand/Desktop/shine-testbed/mlst,target=/usr/src/app/mlst esashineproject/shine-storage

STORAGE= docker inspect -f "{{ .NetworkSettings.IPAddress }}" shine-storage

echo $STORAGE	
echo -e "${COLOR}\n>> Now the NC-shine-backend container will be created.${NC}"
docker run --name NC-shine-backend -idt --network host -e NC_SERVER=192.168.19.142 -e STORAGE_MACHINE=172.17.0.1 --mount type=bind,source=/home/cache,target=/home/cache -d esashineproject/shine-backend

#docker run --name NC-shine-backend -idt -p 5022:22 -p 8100:8100 -p 12345:12345 -e NC_SERVER=192.168.18.142 -e STORAGE_MACHINE=172.17.0.1 -d esashineproject/shine-backend

IDNCClient=NC-shine-backend
docker cp id_rsa.pub ${IDNCClient}:/id_rsa.pub

echo -e "${COLOR}\n>> Now the command ${NC}sh /save_public_key.sh${COLOR} will be run in the $1 container.${NC}"
docker exec -t ${IDNCClient} sh /save_public_key.sh

echo -e "${COLOR}\n>> Now the command ${NC}service ssh start${COLOR} will be run in the $1 container.${NC}"
docker exec -t ${IDNCClient} service ssh start

cat id_rsa.pub >> ~/.ssh/id_rsa.pub
cat id_rsa.pub >> ~/.ssh/authorized_keys
#rm -f id_rsa.pub

echo -e "${COLOR}\n>> Configuring NC client.${NC}"
# CountRepository=$(ls repository | wc -l)
# CountRepository=11
# sed -i "/^nFiles =/s/ =.*/ = $CountRepository/" ConfigFile/clt/configFile.ini

docker cp ConfigFile/clt/configFile.ini NC-shine-backend:/Release/configFile.ini

sleep 3

echo -e "${BOLD}\n>> Launch NC client.${NC}"
docker exec -td NC-shine-backend sh /entry.sh
