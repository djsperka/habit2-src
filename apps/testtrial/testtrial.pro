LIBS += -L../../lib -lhabutil
QT += phonon
CONFIG += qt debug_and_release
INCLUDEPATH += ../../libs/habutil
DESTDIR = ../../bin
HEADERS			=	TestTrialDialog.h
SOURCES			=	main.cpp \
					TestTrialDialog.cpp
					
