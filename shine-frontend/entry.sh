#!/bin/sh
WAZUH_MANAGER=172.17.0.1
#WAZUH_AGENT=`ifconfig eth0 | sed '1,1d' | sed '2,$d' | awk '{ print $2 }'`
#  curl -s -u foo:bar -k -X GET "https://$WAZUH_MANAGER:55000/agents?pretty" | grep $WAZUH_AGENT > /dev/null
#  RES=`echo $?`
#  if [ "$RES" == "1" ];
#  then
#  	echo "Register ..."
#  	/register.sh
#  else
#  	echo "Delete & Register ..."
#  	NUM=`curl -s -u foo:bar -k -X GET "https://$WAZUH_MANAGER:55000/agents?pretty" | grep -n $WAZUH_AGENT | cut -d":" -f1`
#        NUM=`expr $NUM + 14`
#        AGENT_ID=`curl -s -u foo:bar -k -X GET "https://$WAZUH_MANAGER:55000/agents?pretty" | sed "${NUM}q;d" | cut -d":" -f2 | sed 's/\"//g' | xargs`
#	curl -s -u foo:bar -k -X DELETE "https://$WAZUH_MANAGER:55000/agents/$AGENT_ID?pretty"
#  	/register.sh
#  fi
#
#  echo "Initializing Wazuh agent..."
#  /var/ossec/bin/ossec-control start

java -Djava.security.egd=file:/dev/./urandom -Dspring.profiles.active=$PROFILE -jar /app.war