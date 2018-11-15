#!/bin/sh
WAZUH_MANAGER=192.168.19.121
#WAZUH_AGENT=`ifconfig -a | sed '1,1d' | sed '2,$d' | awk '{ print $2 }'`
WAZUH_AGENT=`hostname`
USER="wazuh"
PASSWORD="W4zuh!"

#set -e

  RES=`curl -s -u $USER:$PASSWORD -k -X GET "https://$WAZUH_MANAGER:55000/agents/name/$WAZUH_AGENT?pretty" | grep error | awk '{ print $2 }' | sed 's/,//'`
  if [ "$RES" != "0" ];
  then
  	echo "Register ..."
  	/register.sh
  else
  	echo "Delete & Register ..."
  	AGENT_ID=`curl -s -u $USER:$PASSWORD -k -X GET "https://$WAZUH_MANAGER:55000/agents/name/$WAZUH_AGENT?pretty" | grep "\"id\"" | awk '{ print $2 }' | sed 's/\"//g' | sed 's/,//'`
	curl -s -u $USER:$PASSWORD -k -X DELETE "https://$WAZUH_MANAGER:55000/agents/$AGENT_ID?pretty"
  	/register.sh
  fi

  echo "Initializing Wazuh agent..."
  /var/ossec/bin/ossec-control start

  java -Djava.security.egd=file:/dev/./urandom -Dspring.profiles.active=$PROFILE -jar /app.war
