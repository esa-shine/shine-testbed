FROM ubuntu:14.04

ARG METRICBEAT_VERSION=6.2.4
ARG PACKETBEAT_VERSION=6.2.4
ARG WAZUH_AGENT_VERSION=3.3.1-1

RUN apt-get update
RUN apt-get install -y build-essential openssh-client vim libssh-4 libssh-dev nano gsl-bin libgsl0-dev
COPY log4cpp-1.1.2.tar.gz /
COPY install_log.sh /
RUN sh install_log.sh
ADD utils/lib /usr/lib
COPY fix_lib_server.sh /
COPY generate_keys.sh /
RUN sh fix_lib_server.sh
COPY Shine_srv /
RUN make -C Debug/

# RUN apt-get install -y curl apt-transport-https lsb-release wget
# RUN wget -qO - https://artifacts.elastic.co/GPG-KEY-elasticsearch | sudo apt-key add -
# RUN echo "deb https://artifacts.elastic.co/packages/6.x/apt stable main" | sudo tee -a /etc/apt/sources.list.d/elastic-6.x.list
# RUN sudo apt-get update && sudo apt-get install metricbeat
# RUN sudo apt-get update && sudo apt-get install packetbeat



# RUN curl -s https://packages.wazuh.com/key/GPG-KEY-WAZUH | apt-key add -
# RUN echo "deb https://packages.wazuh.com/3.x/apt/ stable main" | tee /etc/apt/sources.list.d/wazuh.list
# RUN apt-get update
# RUN apt-get install wazuh-agent

# COPY metricbeat.yml /etc/metricbeat/metricbeat.yml

# COPY system.yml /etc/metricbeat/modules.d/system.yml

# COPY logstash-forwarder.crt /etc/pki/tls/certs/logstash-forwarder.crt

# COPY packetbeat.yml /etc/packetbeat/packetbeat.yml

# ADD register.sh /register.sh
# RUN chmod +x /register.sh
# COPY ossec.conf /var/ossec/etc/ossec.conf

# ADD entrypoint.sh /entrypoint.sh
# RUN chmod +x /entrypoint.sh
COPY entry.sh /entry.sh
# # ENTRYPOINT ["/entrypoint.sh"]

# RUN apt-get update && apt-get install -y supervisor
# RUN mkdir -p /var/log/supervisor
# COPY supervisord.conf /etc/supervisord.conf
# CMD ["/usr/bin/supervisord", "--configuration=/etc/supervisord.conf"]




EXPOSE 22
