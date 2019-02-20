TEMPLATE = app
SOURCES = testmisc.cpp
HEADERS = testmisc.h
TARGET = testmisc
CONFIG += qt debug_and_release
QT += sql testlib

QMAKE_CXXFLAGS += -fvisibility=hidden
CONFIG(debug, debug|release) {
	DESTDIR = debug
	LIBS += -L../../libs/habutil/debug -lhabutil
	!win32 {
		PRE_TARGETDEPS += ../../libs/habutil/debug/libhabutil.a
	} else {
		PRE_TARGETDEPS += ../../libs/habutil/debug/habutil.lib
	}
} else {
	DESTDIR = release
	LIBS += -L../../libs/habutil/release -lhabutil
	!win32 {
		PRE_TARGETDEPS += ../../libs/habutil/release/libhabutil.a
	} else {
		PRE_TARGETDEPS += ../../libs/habutil/release/habutil.lib
	}
}

INCLUDEPATH += ../../libs/habutil ../../libs/habutil/habit
DEPENDPATH += ../../libs/habutil ../../libs/habutil/habit

