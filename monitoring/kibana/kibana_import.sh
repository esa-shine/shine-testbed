#!/bin/bash

until nc -z 192.168.200.131 5601;
do
	echo "Kibana not running... Wait" 
	sleep 5
done

ls dashboards | while read LINE;
do
	curl -XPOST 192.168.200.131:5601/api/kibana/dashboards/import -H 'kbn-xsrf:true' -H 'Content-type:application/json' -d @dashboards/${LINE}
done
