#!/bin/sh
WAZUH_MANAGER=192.168.200.135
WAZUH_AGENT=192.168.200.128

#set -e

if [ "$1" = 'start' ]; then
  curl -s -u foo:bar -k -X GET "https://$WAZUH_MANAGER:55000/agents?pretty" | grep $WAZUH_AGENT > /dev/null
  RES=`echo $?`
  if [ "$RES" == "1" ];
  then
  	echo "Register ..."
  	/register.sh
  else
  	echo "Delete & Register ..."
  	NUM=`curl -s -u foo:bar -k -X GET "https://$WAZUH_MANAGER:55000/agents?pretty" | grep -n $WAZUH_AGENT | cut -d":" -f1`
        NUM=`expr $NUM + 14`
        AGENT_ID=`curl -s -u foo:bar -k -X GET "https://$WAZUH_MANAGER:55000/agents?pretty" | sed "${NUM}q;d" | cut -d":" -f2 | sed 's/\"//g' | xargs`
	curl -s -u foo:bar -k -X DELETE "https://$WAZUH_MANAGER:55000/agents/$AGENT_ID?pretty"
  	/register.sh
  fi

  echo "Initializing Wazuh agent..."
  /var/ossec/bin/ossec-control start

  echo "Initializing Metricbeat..."
  /usr/bin/metricbeat -e -v -c /etc/metricbeat/metricbeat.yml -system.hostfs=/hostfs &

  echo "Initializing Packetbeat..."
  /usr/bin/packetbeat -e -v -c /etc/packetbeat/packetbeat.yml

  wait
else
  exec "$@"
fi
