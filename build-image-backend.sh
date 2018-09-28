#!/bin/bash

BOLD='\033[1;34m'
COLOR='\033[0;34m'
NC='\033[0m'

echo -e "${BOLD}\n>> This script builds backend docker image.${NC}"

echo -e "${COLOR}\n>> Now the docker image for shine backend will be created and saved as esashineproject/shine-backend ${NC}"
docker build --tag "esashineproject/shine-backend" shine-backend/.
