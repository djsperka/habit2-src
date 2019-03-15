#!/bin/sh

if [ $# -ne 1 ] ; then
	echo "usage: build-dist.sh tag"
	exit -1
fi

GIT_TAG=$1 

# qt 5.9.1
QT_BIN=/Users/dan/Qt/5.9.1/clang_64/bin/ 

# gstreamer 1.12.4 - standard install location (assuming that's the version installed)
PKG_CONFIG_PATH=/Library/Frameworks/GStreamer.framework/Versions/Current/Libraries/pkgconfig/
GSTREAMER_LIBS_RELOCATED=/Users/dan/gstreamer-1.12.4-relocated.tar
export GIT_TAG QT_BIN PKG_CONFIG_PATH GSTREAMER_LIBS_RELOCATED

make -f macdist.mak package