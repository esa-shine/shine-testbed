#!/bin/bash

BOLD='\033[1;32m'
COLOR='\033[0;32m'
NC='\033[0m'

echo -e "${BOLD}\n>> This script will copy mpd files and medialist.json on ut101 .${NC}"

pass="XCcube"

sshpass -p "$pass" scp -r mpd  opensand@192.168.19.101:/home/opensand/Desktop/
sshpass -p "$pass" scp -r mlst  opensand@192.168.19.101:/home/opensand/Desktop/
sshpass -p "$pass" scp -r ConfigFile/clt  opensand@192.168.19.101:/home/opensand/Desktop/



sshpass -p "$pass" scp -r mpd-s2  opensand@192.168.19.201:/home/opensand/Desktop/mpd/
sshpass -p "$pass" scp -r mlst  opensand@192.168.19.201:/home/opensand/Desktop/
sshpass -p "$pass" scp -r ConfigFile/clt  opensand@192.168.19.201:/home/opensand/Desktop/