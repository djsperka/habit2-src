#!/bin/sh

# receive h264 rtp stream:
#gst-launch-1.0 -v udpsrc port=5000 caps = "application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96" ! rtph264depay ! decodebin ! videoconvert ! osxvideosink
gst-launch-1.0 -v udpsrc port=5000 caps = "application/x-rtp, media=(string)video" ! \
	rtpjitterbuffer ! \
	rtph264depay ! \
	decodebin ! videoconvert ! osxvideosink
