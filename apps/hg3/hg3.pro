# This is a qmake project file, provided as an example on how to use qmake with QtGStreamer.

TEMPLATE = app
TARGET = hg3

# need this for boost?
#INCLUDEPATH += 	"/usr/local/include" 

CONFIG += debug_and_release c++11

QT += sql widgets

# Get gstreamer stuff from this file.
include(../../habit.pri)

# Recommended, to avoid possible issues with the "emit" keyword
# You can otherwise also define QT_NO_EMIT, but notice that this is not a documented Qt macro.
DEFINES += QT_NO_KEYWORDS



CONFIG(debug, debug|release) {
	DESTDIR = debug
	LIBS += -L../../libs/habutil/debug -lhabutil
	PRE_TARGETDEPS += ../../libs/habutil/debug/libhabutil.a
	#QMAKE_POST_LINK = ./uip.sh debug
} else {
	DESTDIR = release
	LIBS += -L../../libs/habutil/release -lhabutil
	PRE_TARGETDEPS += ../../libs/habutil/release/libhabutil.a
	#QMAKE_POST_LINK = ./uip.sh release
}

INCLUDEPATH += ../../libs/habutil 
INCLUDEPATH += ../../libs/habutil/habit ../../libs/habutil/hgst
# use of enum for videotestsrc
#INCLUDEPATH += /Users/dan/install/include/gst-plugins-base-1.12.1
#INCLUDEPATH += /Users/dan/git/gst-plugins-base /Users/dan/git/gst-plugins-good

DEPENDPATH += ../../libs/habutil ../../libs/habutil/habit 

INCLUDEPATH += ../fdhunt
DEPENDPATH += ../fdhunt

# Input
HEADERS += HG3Dialog.h
SOURCES += hg3.cpp HG3Dialog.cpp

