#!/bin/bash

echo -e "\n script per creare contenuti cifrati DASH-enabled usando shaka-packager. Prende in ingresso la durata di ogni segmento ed il nome del file mp4 da segmentare e cifrare."

# NOME_VIDEO_EXTENDED=$1
SEGMENT_DURATION=$1
for i in $( ls /shaka_packager/media/mp4); do

#rimuovo l'estensione dal file..
NOME_VIDEO=$(echo $i | cut -f 1 -d ".")

#echo "$NOME_VIDEO"

#controllo se e' stato inserito anche il parametro per la durata di segmenti; altrimenti, di default, il packager crea contenuti con durata di segmenti 10s.
 if [ -z "$SEGMENT_DURATION" ]
	then 
		echo "non e' stata inserita la durata di segmenti! Di default 10 s. Creo i contenuti cifrati..."
		
		packager input=/shaka_packager/media/mp4/$NOME_VIDEO.mp4,stream=audio,output=/storagedash-s2/$NOME_VIDEO-Audio.mp4 input=/shaka_packager/media/mp4/$NOME_VIDEO.mp4,stream=video,output=/storagedash-s2/$NOME_VIDEO-Video.mp4 --profile on-demand --enable_widevine_encryption --key_server_url "https://license.uat.widevine.com/cenc/getcontentkey/widevine_test" --content_id "3031323334353637" --signer "widevine_test" --aes_signing_key "1ae8ccd0e7985cc0b6203a55855a1034afc252980e970ca90e5202689f947ab9" --aes_signing_iv "d58ce954203b7c9a9a9d467f59839249" --mpd_output mpd/$NOME_VIDEO.mpd

		echo -e "Contenuto $NOME_VIDEO.mp4 cifrato correttamente! Le tracce audio/video cifrate sono $NOME_VIDEO-Audio.mp4 e $NOME_VIDEO-Video.mp4. L'mpd da mettere nel frontend e' $NOME_VIDEO.mpd"
		
	else 
		echo "e' stata inserita la durata di segmenti= $SEGMENT_DURATION. Creo i contenuti cifrati..."
		
		packager input=/shaka_packager/media/mp4/$NOME_VIDEO.mp4,stream=audio,output=/storagedash-s2/$NOME_VIDEO-Audio-$SEGMENT_DURATION-s.mp4 input=/shaka_packager/media/mp4/$NOME_VIDEO.mp4,stream=video,output=/storagedash-s2/$NOME_VIDEO-Video-$SEGMENT_DURATION-s.mp4 --profile on-demand --enable_widevine_encryption --segment_duration $SEGMENT_DURATION --fragment_duration $SEGMENT_DURATION  --key_server_url "https://license.uat.widevine.com/cenc/getcontentkey/widevine_test" --content_id "3031323334353637" --signer "widevine_test" --aes_signing_key "1ae8ccd0e7985cc0b6203a55855a1034afc252980e970ca90e5202689f947ab9" --aes_signing_iv "d58ce954203b7c9a9a9d467f59839249" --mpd_output mpd/$NOME_VIDEO-$SEGMENT_DURATION-s.mpd

		echo -e "\n Contenuto $NOME_VIDEO.mp4 cifrato correttamente! Le tracce audio/video cifrate sono $NOME_VIDEO-Audio-$SEGMENT_DURATION-s.mp4 e $NOME_VIDEO-Video-$SEGMENT_DURATION-s.mp4. L'mpd da mettere nel frontend e' $NOME_VIDEO-$SEGMENT_DURATION-s.mpd"
 fi
#echo "creo e segmento il file $NOME_VIDEO.mp4 con durata di segmento= $SEGMENT_DURATION"
done