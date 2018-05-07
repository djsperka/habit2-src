TARGET = vlcplayer
TEMPLATE = app
QMAKE_CXXFLAGS += -fvisibility=hidden

CONFIG += c++11
QT += sql
lessThan(QT_MAJOR_VERSION, 5) {
    QT += phonon
}
else {
	QT += widgets printsupport
    LIBS += -L/Users/dan/git/vlc-qt/build/libvlc-processed -lvlc -lvlccore
}
CONFIG += qt debug_and_release

QMAKE_LFLAGS += -F/Users/dan/git/vlc-qt/build/src/lib
LIBS       += -framework VLCQtCore -framework VLCQtWidgets
INCLUDEPATH += /Users/dan/install/include /Users/dan/git/vlc-qt/libvlc-headers/include
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
SOURCES			=	vlcplayer.cpp Q5PlayerTestDialog.cpp
HEADERS			=	Q5PlayerTestDialog.h
