#!/bin/bash

BOLD='\033[1;34m'
COLOR='\033[0;34m'
NC='\033[0m'

echo -e "${BOLD}\n>> This script builds satellite NC server docker image.${NC}"

echo -e "${COLOR}\n>> Now the docker image for nc-server-dash will be created and saved as esashineproject/nc-server.${NC}"

docker build --tag "esashineproject/nc-server" $1 nc-server/.
