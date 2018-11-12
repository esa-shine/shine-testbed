#!/bin/bash

BOLD='\033[1;34m'
COLOR='\033[0;34m'
NC='\033[0m'

echo -e "${BOLD}\n>> This script sets metricbeat and packetbeat network.${NC}"

docker network create --ipv6 --driver=bridge --subnet 192.168.210.0/24 --gateway 192.168.210.1 --ip-range 192.168.210.128/25 --subnet 2001:db9:2::/80 -o "com.docker.network.bridge.name"="beats" beats
brctl setageing beats 0

iptables -P FORWARD ACCEPT
iptables -D FORWARD -j DOCKER-ISOLATION

