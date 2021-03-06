FROM node

# Install libraries and utilities
RUN apt-get update && apt-get install -y curl python make g++ build-essential vim ufw

# Get the latest node.js and npm versions and install
#RUN curl -sL https://deb.nodesource.com/setup_6.x | bash -
#RUN apt-get update && apt-get install -y nodejs

# Create app directory
RUN mkdir -p /usr/src/app
WORKDIR /usr/src/app

RUN mkdir -p media
RUN mkdir -p chunks
RUN mkdir -p mlst
RUN mkdir -p mpd

# Install app dependencies
COPY package.json /usr/src/app
#COPY MediaList.json /usr/src/app
RUN npm install
RUN npm install http-proxy ws express formidable  --save

# Bunle app source
COPY . /usr/src/app


ARG WAZUH_AGENT_VERSION=3.6.1-1

# Updating image
RUN apt-get update && apt-get upgrade -y 
RUN apt-get install -y curl apt-transport-https lsb-release wget net-tools gnupg gnupg2 ifstat ethtool vim python-minimal

RUN curl -s https://packages.wazuh.com/key/GPG-KEY-WAZUH | apt-key add -
RUN echo "deb https://packages.wazuh.com/3.x/apt/ stable main" | tee /etc/apt/sources.list.d/wazuh.list
RUN apt-get update
RUN apt-get install -y wazuh-agent=${WAZUH_AGENT_VERSION} libopenscap8 xsltproc libpcap0.8

ADD register.sh /register.sh
RUN chmod +x /register.sh
COPY ossec.conf /var/ossec/etc/ossec.conf

ADD entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]

CMD ["start"]
