openssl req -x509 -batch -nodes -days 3650 -newkey rsa:2048 -keyout logstash_pki/private/logstash-forwarder.key -out logstash_pki/certs/logstash-forwarder.crt -config logstash_openssl.cnf
