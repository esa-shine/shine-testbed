#!/bin/bash

BOLD='\033[1;32m'
COLOR='\033[0;32m'
NC='\033[0m'



echo -e "${COLOR}\n>> Now Running Shine frontend spring boot container.${NC}"
docker run -d --name shine-spring -p 8443:8443 -e "PROFILE=opensand" esashineproject/shine-frontend


echo -e "${COLOR}\n>> Now Running Janus container.${NC}"

docker run -d --name janus --network host  esashineproject/janus
