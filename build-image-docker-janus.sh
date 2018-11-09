#!/bin/bash

BOLD='\033[1;34m'
COLOR='\033[0;34m'
NC='\033[0m'

echo -e "${BOLD}\n>> This script builds janus docker image.${NC}"

echo -e "${COLOR}\n>> Now the janus image will be built${NC}"
docker build --tag "esashineproject/janus" docker-janus/.

