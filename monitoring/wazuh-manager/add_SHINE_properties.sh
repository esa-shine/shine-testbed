curl -XPUT '192.168.200.130:9200/_template/wazuh?pretty' -H 'Content-Type: application/json' -d @wazuh-elastic6-template-alerts.json
#Add to template
#curl -XPUT '192.168.200.130:9200/wazuh-alerts-3.x-*/_mapping/wazuh?pretty' -H 'Content-Type: application/json' -d'
#{ 
#  "properties": { 
#    "data.SHINE.conffilenumber": {
#      "type": "integer" 
#    }, 
#    "data.SHINE.totalpackagenumber": {
#      "type": "integer" 
#    }, 
#    "data.SHINE.usedbandwidth": {
#      "type": "integer" 
#    }, 
#    "data.SHINE.availablebandwidth": {
#      "type": "integer" 
#    }, 
#    "data.SHINE.udpreceived": {
#      "type": "integer" 
#    }, 
#    "data.SHINE.udpreceivederrors": {
#      "type": "integer" 
#    }, 
#    "data.SHINE.tcpsegmentsreceived": {
#      "type": "integer" 
#    }, 
#    "data.SHINE.tcpsegmentsretransmitted": {
#      "type": "integer" 
#    } 
#  } 
#}
#'
curl -XPUT '192.168.200.131:5601/api/saved_objects/index-pattern/wazuh-alerts-3.x-*' -H 'kbn-xsrf:true' -H 'Content-type:application/json' -d @wazuh-index-pattern.json
