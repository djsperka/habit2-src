# This is a qmake project file, provided as an example on how to use qmake with QtGStreamer.

TEMPLATE = app
TARGET = habit2

# need this for boost?
#INCLUDEPATH += 	"/usr/local/include" 

# Tell qmake to use pkg-config to find gstreamer.
CONFIG += debug_and_release c++11

# need sql libs and print support
QT += sql printsupport

# Get gstreamer stuff from this file.
include(../../habit2.pri)

# Now tell qmake to link to gstreamer and also use its include path and Cflags.
#PKGCONFIG += gstreamer-1.0 gstreamer-plugins-base-1.0
# gstreamer-plugins-bad-1.0
QT += widgets

# Recommended, to avoid possible issues with the "emit" keyword
# You can otherwise also define QT_NO_EMIT, but notice that this is not a documented Qt macro.
DEFINES += QT_NO_KEYWORDS

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

macx {
	ICON = ./habit2.icns
	QMAKE_INFO_PLIST = habit.plist
}
win32 {
	RC_ICONS = ../../icons/kids-icon.ico
}

INCLUDEPATH += ../../libs/habutil
INCLUDEPATH += ../../libs/habutil/habit
DEPENDPATH += ../../libs/habutil ../../libs/habutil/habit
SOURCES			=	main.cpp 
HEADERS			=	version.h


