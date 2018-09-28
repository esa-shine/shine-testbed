#!/bin/bash

BOLD='\033[1;34m'
COLOR='\033[0;34m'
NC='\033[0m'

echo -e "${BOLD}\n>> This script builds shaka packager container.${NC}"

echo -e "${COLOR}\n>> Now the DASH packager image will be built using packager:v3${NC}"
docker build --tag "packager:v3" shaka-packager/.

# echo -e "${COLOR}\n>> Now the dashpackager container will be executed in background.${NC}"
# docker run -idt --name dashpackager packager:v3
