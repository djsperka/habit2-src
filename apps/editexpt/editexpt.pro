LIBS += -L../../lib -lhabutil
QT += phonon sql
CONFIG += qt debug_and_release
INCLUDEPATH += ../../libs/habutil ../../libs/habutil/habit
DESTDIR = ../../bin
SOURCES			=	main.cpp EditExptDialog.cpp
HEADERS			=   EditExptDialog.h
					
