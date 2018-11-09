#!/bin/bash

BOLD='\033[1;32m'
COLOR='\033[0;32m'
NC='\033[0m'

echo -e "${COLOR}\n>> Now the dashpackager container will run.${NC}"
docker run -idt --name dashpackager packager:v3



echo -e "${COLOR}\n>> To create DASH content you must enter the container using the following commands and run the last command for each media file:${NC}"

docker exec -it dashpackager sh media/segments-encrypt-contents.sh
