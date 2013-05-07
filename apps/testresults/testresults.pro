TEMPLATE = app
SOURCES = testresults.cpp
HEADERS = testresults.h
TARGET = testresults
CONFIG += qt debug_and_release
QT += phonon sql

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

INCLUDEPATH += ../../libs/habutil ../../libs/habutil/habit
DEPENDPATH += ../../libs/habutil ../../libs/habutil/habit

