#!/bin/bash

ip addr del 10.80.19.33/24 dev eth0
docker network create --ipv6 --driver=bridge --subnet 10.80.19.0/24 --gateway 10.80.19.33 --ip-range 10.80.19.128/28 --subnet 2002:db9:1::/80 -o "com.docker.network.bridge.name"="br0" br0
brctl addif br0 eth0
ip route add default via 10.80.19.1
