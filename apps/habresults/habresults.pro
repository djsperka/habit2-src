TARGET = habresults
TEMPLATE = app
QMAKE_CXXFLAGS += -fvisibility=hidden

QT += sql
lessThan(QT_MAJOR_VERSION, 5) {
    QT += phonon
}
else {
	QT += widgets multimedia printsupport
#	QMAKE_LFLAGS += -F/Users/dan/git/vlc-qt/build/src/lib
#	LIBS       += -framework VLCQtCore -framework VLCQtWidgets
#	INCLUDEPATH += /Users/dan/install/include /Users/dan/git/vlc-qt/libvlc-headers/include
}
CONFIG += qt debug_and_release c++11

CONFIG(debug, debug|release) {
	DESTDIR = debug
	LIBS += -L../../libs/habutil/debug -lhabutil
	PRE_TARGETDEPS += ../../libs/habutil/debug/libhabutil.a
} else {
	DESTDIR = release
	LIBS += -L../../libs/habutil/release -lhabutil
	PRE_TARGETDEPS += ../../libs/habutil/release/libhabutil.a
}

INCLUDEPATH += ../../libs/habutil
INCLUDEPATH += ../../libs/habutil/habit
DEPENDPATH += ../../libs/habutil ../../libs/habutil/habit
SOURCES			=	habresults.cpp 
HEADERS			=
					
