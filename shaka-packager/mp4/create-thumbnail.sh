#!/bin/bash

#ffmpeg -ss 00:00:12 -t 00:00:00.04 -i media/Simply_Red_-_Fake.mp4 -r 25.0 thumbnails/Simply_Red_-_Fake.jpg


BOLD='\033[1;34m'
COLOR='\033[0;34m'
NC='\033[0m'

if [ $# -ne 2 ]
  then
    echo -e "\n>> Please insert file name and second (format=00:00:01).\n"
    exit 1;
fi

FileName=$1
Second=$2
Thumbnail=$(echo $1 | sed 's/.mp4//g')$(echo '.jpg')

echo -e $Thumbnail

echo -e "${COLOR}\n>> Now will be created thumbnail for file $1 at second $2.${NC}"
ffmpeg -ss $2 -t 00:00:00.04 -i $1 -r 25.0 ../thumbnails/$Thumbnail

#ffmpeg -ss $2 -t 00:00:00.04 -i $1 -r 25.0 ../$Thumbnail


# ffmpeg -ss 00:00:12 -t 00:00:00.04 -i Napoli_Campione.webm -r 25.0 Napoli_Campione.jpg