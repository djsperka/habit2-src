LIBS += -L../../lib -lhabutil
CONFIG += qt debug_and_release
QT += phonon
INCLUDEPATH += ../../libs/habutil
DESTDIR = ../../bin
HEADERS			=	PlayerControlDialog.h
SOURCES			=	main.cpp \
					PlayerControlDialog.cpp
					
