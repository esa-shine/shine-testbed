echo "***************************************** TEMPLATE"
curl -XGET '192.168.200.130:9200/_template/*?pretty' -H 'Content-Type: application/json'
echo "***************************************** INDEXES"
curl -XGET '192.168.200.130:9200/wazuh-alerts-*/_mappings/wazuh?pretty' -H 'Content-Type: application/json'
echo "***************************************** INDEX-PATTERN"
curl -XGET '192.168.200.131:5601/api/saved_objects/index-pattern/wazuh-alerts-3.x-*?pretty' -H 'Content-Type: application/json' > wazuh-index-pattern.json
