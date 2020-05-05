#!/bin/sh

#export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig
#pkg-config --libs --cflags gstreamer-1.0
gcc -o avf2 `pkg-config --libs --cflags  gstreamer-1.0` avf2.c