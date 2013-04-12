TARGET = habit
TEMPLATE = app

QT += phonon sql
CONFIG += qt debug_and_release

CONFIG(debug, debug|release) {
	DESTDIR = debug
	LIBS += -L../../libs/habutil/debug -lhabutil
	#PRE_TARGETDEPS += ../../libs/habutil/debug/libhabutil.a
} else {
	DESTDIR = release
	LIBS += -L../../libs/habutil/release -lhabutil
	#PRE_TARGETDEPS += ../../libs/habutil/release/libhabutil.a
}

ICON = ./habiticon.icns
QMAKE_INFO_PLIST = habit.plist


# take care of copying icons into app directory. 
win32 {
}
macx {
}

INCLUDEPATH += ../../libs/habutil
INCLUDEPATH += ../../libs/habutil/habit
DEPENDPATH += ../../libs/habutil ../../libs/habutil/habit
SOURCES			=	main.cpp 
HEADERS			=	version.h
					
