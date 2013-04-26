TEMPLATE = app
SOURCES = testmisc.cpp
HEADERS = testmisc.h
TARGET = testmisc
CONFIG += qt debug_and_release qtestlib
QT += phonon sql

QMAKE_CXXFLAGS += -fvisibility=hidden
CONFIG(debug, debug|release) {
	LIBS += -L../../libs/habutil/debug -lhabutil
} else {
	LIBS += -L../../libs/habutil/release -lhabutil
}

INCLUDEPATH += ../../libs/habutil ../../libs/habutil/habit
DEPENDPATH += ../../libs/habutil ../../libs/habutil/habit

