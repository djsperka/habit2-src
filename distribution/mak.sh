#!/bin/bash

GIT_TAG=master
QT_BIN=/Users/dan/Qt/5.14.2/5.14.2/clang_64/bin
PKG_CONFIG_PATH=/Library/Frameworks/GStreamer.framework/Versions/Current/lib/pkgconfig/
SIGUSERID=WWAFVH26T8
export GIT_TAG QT_BIN PKG_CONFIG_PATH SIGUSERID
make -f reloc-macdist.mak reloc
