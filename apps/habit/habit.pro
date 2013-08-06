TARGET = habit
TEMPLATE = app
QMAKE_CXXFLAGS += -fvisibility=hidden

CONFIG += qt debug_and_release using_qt5

using_qt5 {
	QT += widgets multimedia multimediawidgets printsupport sql
	DEFINES += USING_QT5
} else {
	QT += phonon sql
}

CONFIG(debug, debug|release) {
	DESTDIR = debug
	LIBS += -L../../libs/habutil/debug -lhabutil
	PRE_TARGETDEPS += ../../libs/habutil/debug/libhabutil.a
} else {
	DESTDIR = release
	LIBS += -L../../libs/habutil/release -lhabutil
	PRE_TARGETDEPS += ../../libs/habutil/release/libhabutil.a
}

ICON = ./habiticon.icns
QMAKE_INFO_PLIST = habit.plist

# take care of copying icons into app directory. 
win32 {
}
macx {
#    IconFiles.files = habit-large.png habit-small.png
#    IconFiles.path = Contents/MacOS
#    QMAKE_BUNDLE_DATA += MediaFiles
}

INCLUDEPATH += ../../libs/habutil
INCLUDEPATH += ../../libs/habutil/habit
DEPENDPATH += ../../libs/habutil ../../libs/habutil/habit
SOURCES			=	main.cpp 
HEADERS			=	version.h
					
