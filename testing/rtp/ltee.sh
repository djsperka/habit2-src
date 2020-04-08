#!/bin/sh

if [ $# -ne 3 ]; then
	echo "usage: ltee.sh device-id host-or-zero outfile"
	exit -1
fi

DEVID=$1
if [ "$2" == "0" ]; then
	TOHOST="127.0.0.1"
	TOPORT=5000
	VSINK="osxvideosink async=false"
else
	TOHOST=$2
	TOPORT=5000
	VSINK="x264enc bitrate=500 ! rtph264pay ! udpsink host=${TOHOST} port=5000"
fi
OUTFILE=$3

# async=false on videosink?

gst-launch-1.0 -e avfvideosrc do-timestamp=true device-index=${DEVID} ! timeoverlay ! videoscale ! videorate ! \
	video/x-raw,width=640,height=360,framerate=10/1 ! \
	tee name=eeT ! \
	queue leaky=downstream ! videoconvert ! videoscale ! ${VSINK} \
	eeT. ! queue leaky=downstream ! videoconvert ! queue ! x264enc bitrate=500 ! mp4mux ! filesink location=$OUTFILE




