#!/bin/sh

# works 
#gst-launch-1.0 -e videotestsrc pattern=ball do-timestamp=true is-live=true ! timeoverlay ! 'video/x-raw,width=704,height=576,framerate=25/1' ! tee name=liveTee ! \
#	queue leaky=downstream ! videoconvert ! osxvideosink async=false \
#	liveTee. ! queue leaky=downstream ! videoconvert ! queue ! x264enc ! mp4mux ! filesink location=/tmp/test.mp4

if [ $# -ne 1 ]; then
	echo "usage: launch2.sh cam-index[0,1,..]"
	exit -1
fi

USEHOST=192.168.0.11
CAMIND=$1
gst-launch-1.0 -e avfvideosrc do-timestamp=true device-index=${CAMIND} ! \
				timeoverlay ! \
				videoscale ! \
				videoconvert ! \
				x264enc tune=zerolatency bitrate=500 speed-preset=superfast ! \
   				rtph264pay ! \
   				udpsink host=${USEHOST} port=5000
