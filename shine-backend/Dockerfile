FROM ubuntu:14.04
RUN apt-get update 
RUN apt-get install -y build-essential openssh-server vim gsl-bin libgsl0-dev
RUN mkdir /var/run/sshd
RUN mkdir /home/cache
RUN mkdir ~/.ssh
RUN touch ~/.ssh/authorized_keys
RUN touch ~/.ssh/id_rsa.pub
RUN echo 'root:screencast' | chpasswd
RUN sed -i 's/PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config
# SSH login fix. Otherwise user is kicked off after login
RUN sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd
ENV NOTVISIBLE "in users profile"
RUN echo "export VISIBLE=now" >> /etc/profile
COPY log4cpp-1.1.2.tar.gz /
COPY install_log.sh /

RUN sh install_log.sh
ADD utils/lib/ /usr/lib/
COPY fix_lib_client.sh /
RUN sh fix_lib_client.sh
COPY save_public_key.sh /
COPY Shine_clt /
RUN make -C Release/

# Start code for NodeJS
RUN apt-get install -y curl python make g++ ufw
RUN curl -sL https://deb.nodesource.com/setup_6.x | bash -
RUN apt-get update && apt-get install -y nodejs
RUN mkdir -p /usr/src/app
WORKDIR /usr/src/app
COPY package.json /usr/src/app
RUN npm install
RUN npm install request ws watchr --save
#RUN npm install request ws chokidar --save
COPY server.js /usr/src/app
# End code for NodeJS


#RUN echo "service ssh start" > /root/.bashrc
#WORKDIR /
#CMD    ["/usr/sbin/sshd", "-D"]
 COPY entry.sh /entry.sh
# End code for NodeJS
# ENTRYPOINT [ "/entry.sh" ]

CMD [ "npm", "start" ]

# RUN apt-get update && apt-get install -y supervisor
# RUN mkdir -p /var/log/supervisor
# COPY supervisord.conf /etc/supervisord.conf
# CMD ["/usr/bin/supervisord", "--configuration=/etc/supervisord.conf"]
EXPOSE 22