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

# EXPOSE 8080
CMD [ "npm", "start" ]
