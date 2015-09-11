CONFIG += qt debug_and_release
QT += multimedia phonon sql
INCLUDEPATH += ../../libs/habutil ../../libs/habutil/habit
PRE_TARGETDEPS += ../../libs/habutil/release/libhabutil.a

CONFIG(debug, debug|release) {
	DESTDIR = debug
	LIBS += -L../../libs/habutil/debug -lhabutil
	PRE_TARGETDEPS += ../../libs/habutil/debug/libhabutil.a
} else {
	DESTDIR = release
	LIBS += -L../../libs/habutil/release -lhabutil
	PRE_TARGETDEPS += ../../libs/habutil/release/libhabutil.a
}
HEADERS			=	TestKPDialog.h
SOURCES			=	main.cpp TestKPDialog.cpp
					
