#!/bin/bash

wget --content-disposition http://192.168.200.131:5601/api/kibana/dashboards/export?dashboard=$1

#curl -XGET localhost:5601/api/kibana/dashboards/export?dashboard=[dashboard_uuid] > my-dashboards.json
