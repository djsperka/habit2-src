TARGET = gstsp
TEMPLATE = app
QMAKE_CXXFLAGS += -fvisibility=hidden

QT += sql widgets printsupport
CONFIG += qt debug_and_release c++11

# Get gstreamer stuff from this file.
include(../../habit2.pri)

CONFIG(debug, debug|release) {
	DESTDIR = debug
	!win32 {
		LIBS += -L/usr/local/lib -lboost_filesystem -L../../libs/habutil/debug -lhabutil
		PRE_TARGETDEPS += ../../libs/habutil/debug/libhabutil.a
	} else {
		LIBS += -L/usr/local/lib -lboost_filesystem -L../../libs/habutil/debug -lhabutil
		PRE_TARGETDEPS += ../../libs/habutil/debug/habutil.lib
	}
	DEFINES += HABIT_DEBUG
} else {
	DESTDIR = release
	!win32 {
		LIBS += -L/usr/local/lib -lboost_filesystem -L../../libs/habutil/release -lhabutil
		PRE_TARGETDEPS += ../../libs/habutil/release/libhabutil.a
	} else {
		LIBS += -L../../libs/habutil/release -lhabutil
		PRE_TARGETDEPS += ../../libs/habutil/release/habutil.lib
	}
	DEFINES += HABIT_RELEASE
}

INCLUDEPATH += 	../../libs/habutil \
				../../libs/habutil/habit \
				../../libs/habutil/hgst \
				/usr/local/include

DEPENDPATH += ../../libs/habutil ../../libs/habutil/habit ../../libs/habutil/hgst 
SOURCES			=	gstsp.cpp MM1.cpp
HEADERS			=	MM1.h
