#!/bin/bash

BOLD='\033[1;34m'
COLOR='\033[0;34m'
NC='\033[0m'

echo -e "${BOLD}\n>> This script builds storage docker image.${NC}"

echo -e "${COLOR}\n>> Now the docker image for shine storage server will be created and saved as esashineproject/shine-storage ${NC}"
docker build --tag "esashineproject/shine-storage" shine-storage/.
