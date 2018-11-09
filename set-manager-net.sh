#!/bin/bash

BOLD='\033[1;34m'
COLOR='\033[0;34m'
NC='\033[0m'

echo -e "${BOLD}\n>> This script sets the monitoring manager network.${NC}"

docker network create --ipv6 --driver=bridge --subnet 192.168.200.0/24 --gateway 192.168.200.1 --ip-range 192.168.200.128/25 --subnet 2001:db9:1::/80 -o "com.docker.network.bridge.name"="server" server
brctl setageing server 0

