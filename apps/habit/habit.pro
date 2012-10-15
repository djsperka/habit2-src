TARGET = habit
TEMPLATE = app

QT += phonon sql
CONFIG += qt debug_and_release

CONFIG(debug) {
	LIBS += -L../../libs/habutil/build/Debug -lhabutil
	PRE_TARGETDEPS += ../../libs/habutil/build/Debug/libhabutil.a
} else {
	LIBS += -L../../libs/habutil/build/Release -lhabutil
	PRE_TARGETDEPS += ../../libs/habutil/build/Release/libhabutil.a
}

INCLUDEPATH += ../../libs/habutil
INCLUDEPATH += ../../libs/habutil/habit
DEPENDPATH += ../../libs/habutil ../../libs/habutil/habit
DESTDIR = ../../bin
SOURCES			=	main.cpp 
HEADERS			=	version.h
					
