#FROM openjdk:8-jdk-alpine
FROM openjdk:8
# Install gstreamer and opencv dependencies
RUN \
    apt-get update && apt-get upgrade -y && \
    apt-get install -y libgstreamer1.0-0 \
            gstreamer1.0-plugins-base \
            gstreamer1.0-plugins-good \
            gstreamer1.0-plugins-bad \
            gstreamer1.0-plugins-ugly \
            gstreamer1.0-libav \
            gstreamer1.0-doc \
            gstreamer1.0-tools \
            libgstreamer1.0-dev \
            libgstreamer-plugins-base1.0-dev
######wazuh agent
#RUN apt-get install -y curl apt-transport-https lsb-release wget
#RUN curl -s https://packages.wazuh.com/key/GPG-KEY-WAZUH | apt-key add -
#RUN echo "deb https://packages.wazuh.com/3.x/apt/ stable main" | tee /etc/apt/sources.list.d/wazuh.list
#RUN apt-get update
#RUN apt-get install wazuh-agent
#
#ADD register.sh /register.sh
#RUN chmod +x /register.sh
#COPY ossec.conf /var/ossec/etc/ossec.conf
#########

VOLUME /tmp
COPY spring-boot-shine-frontend-1.0.war app.war
COPY keystore.jks keystore.jks/
COPY keystore.p12  keystore.p12
ENV PROFILE default
COPY entry.sh /entry.sh
#ENTRYPOINT ["java","-Djava.security.egd=file:/dev/./urandom","-Dspring.profiles.active=$PROFILE","-jar","/app.war"]
ENTRYPOINT ["/entry.sh"]