#!/bin/bash

docker network create --ipv6 --driver=bridge --subnet 192.168.200.0/24 --gateway 192.168.200.1 --ip-range 192.168.200.128/25 --subnet 2001:db9:1::/80 -o "com.docker.network.bridge.name"="server" server
brctl setageing server 0
docker network create --ipv6 --driver=bridge --subnet 192.168.210.0/24 --gateway 192.168.210.1 --ip-range 192.168.210.128/25 --subnet 2001:db9:2::/80 -o "com.docker.network.bridge.name"="beats" beats
brctl setageing beats 0
docker network create --ipv6 --driver=bridge --subnet 192.168.220.0/24 --gateway 192.168.220.1 --ip-range 192.168.220.128/25 --subnet 2001:db9:3::/80 -o "com.docker.network.bridge.name"="agents" agents
brctl setageing agents 0

iptables -P FORWARD ACCEPT
iptables -D FORWARD -j DOCKER-ISOLATION

