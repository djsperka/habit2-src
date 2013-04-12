
QT += phonon sql
CONFIG += qt debug_and_release

CONFIG(debug, debug|release) {
	DESTDIR = debug
	LIBS += -L../../libs/habutil/debug -lhabutil
} else {
	DESTDIR = release
	LIBS += -L../../libs/habutil/release -lhabutil
}


INCLUDEPATH += ../../libs/habutil ../../libs/habutil/habit
DEPENDPATH += ../../libs/habutil ../../libs/habutil/habit

SOURCES			=	main.cpp
HEADERS			= 
					
