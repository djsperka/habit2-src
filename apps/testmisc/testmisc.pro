TEMPLATE = app
SOURCES = testmisc.cpp
HEADERS = testmisc.h
TARGET = testmisc

CONFIG(debug) {
	LIBS += -L../../libs/habutil/build/Debug -lhabutil
	PRE_TARGETDEPS += ../../libs/habutil/build/Debug/libhabutil.a
} else {
	LIBS += -L../../libs/habutil/build/Release -lhabutil
	PRE_TARGETDEPS += ../../libs/habutil/build/Release/libhabutil.a
}

CONFIG += qt debug_and_release qtestlib
QT += phonon
INCLUDEPATH += ../../libs/habutil ../../libs/habutil/habit
DEPENDPATH += ../../libs/habutil ../../libs/habutil/habit
DESTDIR = ../../bin

# install
INSTALLS += target 

