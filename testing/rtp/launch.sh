#!/bin/sh

USEHOST=192.168.0.11
#gst-launch-1.0 -v avfvideosrc device-index=1 ! \
#   video/x-raw,framerate=10/1,width=320,height=240 ! \
#   videoscale ! videoconvert ! \
#   x264enc tune=zerolatency bitrate=500 speed-preset=superfast ! \
#   rtph264pay ! \
#   udpsink host=${USEHOST} port=5000


#if [ $# -eq 1 -a $1 -eq 1 ] then
   
gst-launch-1.0 -e -v avfvideosrc device-index=0 ! videobalance saturation=0.0 ! tee name=tee0 ! \
   video/x-raw,framerate=10/1,width=320,height=240 ! \
   videoscale ! videoconvert ! \
   x264enc tune=zerolatency bitrate=500 speed-preset=superfast ! \
   rtph264pay ! \
   udpsink host=${USEHOST} port=5000 \
   tee0. ! x264enc speed-preset=6 ! video/x-h264,profile=baseline ! \
   qtmux ! filesink location=recording2.mov
  