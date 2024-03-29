# This is a qmake project file, provided as an example on how to use qmake with QtGStreamer.

TEMPLATE = app
TARGET = fdhunt

# need this for boost?
#INCLUDEPATH += 	"/usr/local/include" 
CONFIG += debug_and_release c++11

#PKGCONFIG += gstreamer-1.0
QT += widgets

# Recommended, to avoid possible issues with the "emit" keyword
# You can otherwise also define QT_NO_EMIT, but notice that this is not a documented Qt macro.
DEFINES += QT_NO_KEYWORDS

# Get gstreamer stuff from this file.
include(../../habit2.pri)


CONFIG(debug, debug|release) {
	DESTDIR = debug
	!win32 {
		LIBS += -L../../libs/habutil/debug -lhabutil
		PRE_TARGETDEPS += ../../libs/habutil/debug/libhabutil.a
	} else {
		LIBS += -L../../libs/habutil/debug -lhabutil
		PRE_TARGETDEPS += ../../libs/habutil/debug/habutil.lib
	}
	DEFINES += HABIT_DEBUG
} else {
	DESTDIR = release
	!win32 {
		LIBS += -L../../libs/habutil/release -lhabutil
		PRE_TARGETDEPS += ../../libs/habutil/release/libhabutil.a
	} else {
		LIBS += -L../../libs/habutil/release -lhabutil
		PRE_TARGETDEPS += ../../libs/habutil/release/habutil.lib
	}
	DEFINES += HABIT_RELEASE
}

INCLUDEPATH += 	../../libs/habutil/habit \
				../../libs/habutil/hgst
# use of enum for videotestsrc
#INCLUDEPATH += /Users/dan/install/include/gst-plugins-base-1.12.1
#INCLUDEPATH += /Users/dan/git/gst-plugins-base /Users/dan/git/gst-plugins-good

DEPENDPATH += ../../libs/habutil ../../libs/habutil/habit ../../libs/habutil/hgst 


# Input
HEADERS += FDHuntDialog.h FDDialog.h
SOURCES += fdhunt.cpp FDHuntDialog.cpp FDDialog.cpp

