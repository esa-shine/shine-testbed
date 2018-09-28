#!/bin/sh
WAZUH_MANAGER=172.18.0.10
WAZUH_AGENT=172.18.0.2

#set -e
  curl -s -u wazuh:W4zuh! -k -X GET "https://$WAZUH_MANAGER:55000/agents?pretty" | grep $WAZUH_AGENT > /dev/null
  RES=`echo $?`
  if [ "$RES" == "1" ];
  then
  	echo "Register ..."
  	/register.sh
  else
  	echo "Delete & Register ..."
  	NUM=`curl -s -u wazuh:W4zuh! -k -X GET "https://$WAZUH_MANAGER:55000/agents?pretty" | grep -n $WAZUH_AGENT | cut -d":" -f1`
        NUM=`expr $NUM + 14`
        AGENT_ID=`curl -s -u wazuh:W4zuh! -k -X GET "https://$WAZUH_MANAGER:55000/agents?pretty" | sed "${NUM}q;d" | cut -d":" -f2 | sed 's/\"//g' | xargs`
	curl -s -u wazuh:W4zuh! -k -X DELETE "https://$WAZUH_MANAGER:55000/agents/$AGENT_ID?pretty"
  	/register.sh
  fi

  echo "Initializing Wazuh agent..."
  /var/ossec/bin/ossec-control start

  echo "Initializing Metricbeat..."
  /usr/bin/metricbeat -e -v -c /etc/metricbeat/metricbeat.yml &

  echo "Initializing Packetbeat..."
  /usr/bin/packetbeat -e -v -c /etc/packetbeat/packetbeat.yml
  wait
