#!/bin/bash

BOLD='\033[1;32m'
COLOR='\033[0;32m'
NC='\033[0m'

echo -e "${BOLD}\n>> This script will run network coding server container.${NC}"

# echo -e "${COLOR}\n>> Now the shineNCnetwork overlay network will be created.${NC}"
# docker network create shineNCnetwork


echo -e "${COLOR}\n>> Now the NC server containers will be created using esashineproject/nc-server.${NC}"
docker run --name NC-server -idt -d --hostname nc-server --network host esashineproject/nc-server

IDNCServer=$(docker ps -q --filter name="NC-server")


echo -e "${COLOR}\n>> Now the SSH public and private keys will be generated in server containers.${NC}"
docker exec -t ${IDNCServer} sh generate_keys.sh


echo -e "${COLOR}\n>> Now the repository filder will be copied in NC server containers.${NC}"
docker cp repository ${IDNCServer}:/Debug

echo -e "${COLOR}\n>> Now the file configFile.ini of NC server will be updated with the number of files.${NC}"

CountRepository=$(ls repository | wc -l)
sed -i "/^nFiles =/s/ =.*/ = $CountRepository/" ConfigFile/srv/configFile.ini
docker cp ConfigFile/srv/configFile.ini ${IDNCServer}:/Debug/configFile.ini
sed -i "/^nFiles =/s/ =.*/ = $CountRepository/" ConfigFile/clt/configFile.ini
docker cp ${IDNCServer}:id_rsa.pub .

pass="XCcube"

sshpass -p "$pass" scp id_rsa.pub  opensand@192.168.19.101:/home/opensand/Desktop

sshpass -p "$pass" scp -P 19000 id_rsa.pub  opensand@192.168.19.110:/home/opensand/Desktop



echo -e "${BOLD}\n>> Launch NC server.${NC}"

docker exec -td ${IDNCServer} bash entry.sh

