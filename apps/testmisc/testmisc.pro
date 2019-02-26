TEMPLATE = app
SOURCES = testmisc.cpp TestPreroller.cpp
HEADERS = testmisc.h TestPreroller.h
TARGET = testmisc
CONFIG += qt debug_and_release
QT += sql testlib widgets

# Get gstreamer stuff from this file.
include(../../habit2.pri)

QMAKE_CXXFLAGS += -fvisibility=hidden
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

INCLUDEPATH += ../../libs/habutil ../../libs/habutil/habit ../../libs/habutil/hgst
DEPENDPATH += ../../libs/habutil ../../libs/habutil/habit ../../libs/habutil/hgst

