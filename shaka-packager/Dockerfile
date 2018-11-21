FROM google/shaka-packager
RUN apt-get update && apt-get install -y vim
RUN mkdir /shaka_packager/media
RUN mkdir /shaka_packager/media/mp4/
COPY mp4 /shaka_packager/media/mp4/
ADD segments-encrypt-contents.sh /shaka_packager/media/
ADD segments-encrypt-contents.v2.sh /shaka_packager/media/
RUN mkdir /shaka_packager/media/encrypted/
RUN mkdir /shaka_packager/media/mpd/
RUN mkdir /shaka_packager/media/mpd-s2/
