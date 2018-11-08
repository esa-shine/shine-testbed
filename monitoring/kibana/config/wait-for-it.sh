#!/bin/bash

set -e

host=`echo $1 | cut -d":" -f1`
wazuh=`echo $1 | cut -d":" -f2`
shift
cmd="/usr/local/bin/kibana-docker"

until curl -XGET $host:9200; do
  >&2 echo "Elastic is unavailable - sleeping"
  sleep 5
done

>&2 echo "Elastic is up - executing command"

#Insert default templates
cat /usr/share/kibana/config/wazuh-elastic6-template-alerts.json | curl -XPUT "http://${host}:9200/_template/wazuh" -H 'Content-Type: application/json' -d @-
sleep 5

# Wazuh password echo -n password | base64
>&2 echo "Setting API credentials into Wazuh APP"
CONFIG_CODE=$(curl -s -o /dev/null -w "%{http_code}" -XGET http://$host:9200/.wazuh/wazuh-configuration/1513629884013)
if [ "x$CONFIG_CODE" = "x404" ]; then
  curl -s -XPOST http://$host:9200/.wazuh/wazuh-configuration/1513629884013 -H 'Content-Type: application/json' -d'
    {
      "api_user": "wazuh",
      "api_password": "VzR6dWgh",
      "url": "https://'${wazuh}'",
      "api_port": "55000",
      "insecure": "true",
      "component": "API",
      "cluster_info": {
        "manager": "wazuh-manager",
        "cluster": "Disabled",
        "status": "disabled"
       },
      "extensions": {
        "oscap": true,
        "audit": true,
        "pci": false,
        "aws": false,
        "virustotal": false,
        "gdpr": false,
        "ciscat": false
      }
    }
    '
else
  >&2 echo "Wazuh APP already configured"
fi

sleep 5

exec $cmd
