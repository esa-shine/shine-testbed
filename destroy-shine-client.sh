#!/bin/bash

BOLD='\033[1;32m'
COLOR='\033[0;32m'
NC='\033[0m'

echo -e "${COLOR}\n>> Now stopping all shine container.${NC}"


docker rm -f NC-shine-backend shine-storage


echo -e "${COLOR}\n>> Now cleaning up cache directory.${NC}"
cd /home/cache
rm *
