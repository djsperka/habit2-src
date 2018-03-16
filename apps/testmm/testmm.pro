TARGET = testmm
TEMPLATE = app
QMAKE_CXXFLAGS += -fvisibility=hidden

QT += sql widgets multimedia printsupport
CONFIG += qt debug_and_release c++11

#PKGCONFIG += gstreamer-1.0 gstreamer-plugins-base-1.0

# Get gstreamer stuff from this file.
include(../../habit.pri)

CONFIG(debug, debug|release) {
	DESTDIR = debug
	LIBS += -L../../libs/habutil/debug -lhabutil
	PRE_TARGETDEPS += ../../libs/habutil/debug/libhabutil.a
} else {
	DESTDIR = release
	LIBS += -L../../libs/habutil/release -lhabutil
	PRE_TARGETDEPS += ../../libs/habutil/release/libhabutil.a
}

INCLUDEPATH += 	"/usr/local/include" \ 
				../../libs/habutil \
				../../libs/habutil/habit \
				../../libs/habutil/hgst

DEPENDPATH += ../../libs/habutil ../../libs/habutil/habit ../../libs/habutil/hgst 
SOURCES			=	main.cpp TestMMDialog.cpp TestMMController.cpp
HEADERS			=	TestMMDialog.h TestMMController.h
