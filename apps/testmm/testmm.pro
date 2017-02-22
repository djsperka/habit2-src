TARGET = testmm
TEMPLATE = app
QMAKE_CXXFLAGS += -fvisibility=hidden

QT += phonon sql
CONFIG += qt debug_and_release

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
SOURCES			=	main.cpp TestMMDialog.cpp TestMMController.cpp
HEADERS			=	TestMMDialog.h TestMMController.h
