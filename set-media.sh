#!/bin/bash

BOLD='\033[1;32m'
COLOR='\033[0;32m'
NC='\033[0m'

echo -e "${BOLD}\n>> This script sets all the media for SHINE.${NC}"

# Removing old folders
rm -rf repository
rm -rf mpd/

echo -e "${COLOR}\n>> Now the 'media' and 'mpd' folders will be copied in local from shaka packager container.${NC}"
IDPackager=$(docker ps -q --filter name="dashpackager")
docker cp ${IDPackager}:/storagedash .
cp -R storagedash/* mp4
rm -R storagedash
docker cp ${IDPackager}:/shaka_packager/mpd .

echo -e "${COLOR}\n>> Now the files MediaList.json will be created.${NC}"

segmentSize=64000;
touch mlst/MediaList.json
echo '{' > 'mlst/MediaList.json'

j=0
echo -e "\t"\"$j""\"": { " >> 'mlst/MediaList.json'
echo -e "\t\t"\""name"\"": "\""$j"\""," >> 'mlst/MediaList.json'
echo -e "\t\t"\""value"\"": 0," >> 'mlst/MediaList.json'
echo -e "\t\t"\""size"\"": 0," >> 'mlst/MediaList.json'
echo -e "\t\t"\""numChunks"\"": 0" >> 'mlst/MediaList.json'
echo -e "\t}," >> 'mlst/MediaList.json'

j=$((j+1))
for i in $( ls mp4); do
	# Setting MediaList.json
	echo -e "Working on item $j: $i"
	printf -v nj "%02d" $j
	echo -e "\t"\"$j""\"": { " >> 'mlst/MediaList.json'
	echo -e "\t\t"\""name"\"": "\""$i"\""," >> 'mlst/MediaList.json'
	echo -e "\t\t"\""value"\"": 0," >> 'mlst/MediaList.json'
	size=$(ls -l mp4/$i | awk '{print $5}')
	echo -e "\t\t"\""size"\"": $size," >> 'mlst/MediaList.json'
	numChunks=$(($size/$segmentSize + 1))
	echo -e "\t\t"\""numChunks"\"": $numChunks" >> 'mlst/MediaList.json'
	echo -e "\t}," >> 'mlst/MediaList.json'

	j=$((j+1))
done

for i in $( ls webm); do
	# Setting MediaList.json
	echo -e "Working on item $j: $i"
	printf -v nj "%02d" $j
	echo -e "\t"\"$j""\"": { " >> 'mlst/MediaList.json'
	echo -e "\t\t"\""name"\"": "\""$i"\""," >> 'mlst/MediaList.json'
	echo -e "\t\t"\""value"\"": 0," >> 'mlst/MediaList.json'
	size=$(ls -l webm/$i | awk '{print $5}')
	echo -e "\t\t"\""size"\"": $size," >> 'mlst/MediaList.json'
	numChunks=$(($size/$segmentSize + 1))
	echo -e "\t\t"\""numChunks"\"": $numChunks" >> 'mlst/MediaList.json'
	echo -e "\t}," >> 'mlst/MediaList.json'

	j=$((j+1))
done

sed -i '$ s/.$//' mlst/MediaList.json

echo '}' >> 'mlst/MediaList.json'

echo -e "${COLOR}\n>> Now the files in 'repository' folder will be set.${NC}"

mkdir repository

cp 0.mp4 repository
j=1
for i in $( ls mp4 ); do
	echo -e "Working on item $j: $i"
	printf -v nj "%02d" $j #00, 01, 02 ... 99
	cp mp4/$i repository/$nj.mp4
	j=$((j+1))
done

for i in $( ls webm ); do
	echo -e "Working on item $j: $i"
	printf -v nj "%02d" $j #00, 01, 02 ... 99
	cp webm/$i repository/$nj.webm
	j=$((j+1))
done
