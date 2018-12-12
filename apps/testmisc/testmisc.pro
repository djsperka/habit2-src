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
	LIBS += -L../../libs/habutil/debug -lhabutil
	PRE_TARGETDEPS += ../../libs/habutil/debug/libhabutil.a
} else {
	DESTDIR = release
	LIBS += -L../../libs/habutil/release -lhabutil
	PRE_TARGETDEPS += ../../libs/habutil/release/libhabutil.a
}

INCLUDEPATH += ../../libs/habutil ../../libs/habutil/habit ../../libs/habutil/hgst
DEPENDPATH += ../../libs/habutil ../../libs/habutil/habit ../../libs/habutil/hgst

