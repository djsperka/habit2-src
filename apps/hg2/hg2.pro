# This is a qmake project file, provided as an example on how to use qmake with QtGStreamer.

TEMPLATE = app
TARGET = hg2

# need this for boost?
INCLUDEPATH += 	"/usr/local/include" 
#INCLUDEPATH += "/Users/dan/install/include/gst-plugins-base-1.12.1"

# produce nice compilation output
#CONFIG += silent

# Tell qmake to use pkg-config to find QtGStreamer.
CONFIG += debug_and_release link_pkgconfig c++11

# need sql libs
QT += sql

#QMAKE_FLAGS += "-Wl,-rpath,\'/Users/dan/install/lib' -Wl,-rpath,\'\$$ORIGIN\'"

PKGCONFIG += gstreamer-1.0
QT += widgets


# Recommended if you are using g++ 4.5 or later. Must be removed for other compilers.
#QMAKE_CXXFLAGS += -std=c++0x

# Recommended, to avoid possible issues with the "emit" keyword
# You can otherwise also define QT_NO_EMIT, but notice that this is not a documented Qt macro.
DEFINES += QT_NO_KEYWORDS



CONFIG(debug, debug|release) {
	DESTDIR = debug
	LIBS += -L../../libs/habutil/debug -lhabutil
	PRE_TARGETDEPS += ../../libs/habutil/debug/libhabutil.a
	QMAKE_POST_LINK = ./uip.sh debug
} else {
	DESTDIR = release
	LIBS += -L../../libs/habutil/release -lhabutil
	PRE_TARGETDEPS += ../../libs/habutil/release/libhabutil.a
	QMAKE_POST_LINK = ./uip.sh release
}

INCLUDEPATH += ../../libs/habutil 
INCLUDEPATH += ../../libs/habutil/habit ../../libs/habutil/hgst
DEPENDPATH += ../../libs/habutil ../../libs/habutil/habit 


# Input
HEADERS += HG2Dialog.h
SOURCES += hg2.cpp HG2Dialog.cpp

