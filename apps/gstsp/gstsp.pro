TARGET = gstsp
TEMPLATE = app
QMAKE_CXXFLAGS += -fvisibility=hidden

QT += sql widgets printsupport multimedia
CONFIG += qt debug_and_release c++11

# Get gstreamer stuff from this file.
include(../../habit2.pri)

# Info.plist file on Mac

macx {
    QMAKE_INFO_PLIST = gstsp.plist
}

CONFIG(debug, debug|release) {
	DESTDIR = debug
	!win32 {
		LIBS += -L/usr/local/lib -lboost_filesystem -lboost_system -L../../libs/habutil/debug -lhabutil
		PRE_TARGETDEPS += ../../libs/habutil/debug/libhabutil.a
	} else {
		LIBS += -L/usr/local/lib -lboost_filesystem -L../../libs/habutil/debug -lhabutil
		PRE_TARGETDEPS += ../../libs/habutil/debug/habutil.lib
	}
	DEFINES += HABIT_DEBUG
} else {
	DESTDIR = release
	!win32 {
		LIBS += -L/usr/local/lib -lboost_filesystem -L../../libs/habutil/release -lhabutil
		PRE_TARGETDEPS += ../../libs/habutil/release/libhabutil.a
	} else {
		LIBS += -L../../libs/habutil/release -lhabutil
		PRE_TARGETDEPS += ../../libs/habutil/release/habutil.lib
	}
	DEFINES += HABIT_RELEASE
}

INCLUDEPATH += 	../../libs/habutil \
				../../libs/habutil/habit \
				../../libs/habutil/hgst \
				/usr/local/include

DEPENDPATH += ../../libs/habutil ../../libs/habutil/habit ../../libs/habutil/hgst 
SOURCES			=	gstsp.cpp  HMM.cpp Port.cpp Source.cpp Stim.cpp Counter.cpp GstspDialog.cpp HabitStimFactory.cpp JsonStimFactory.cpp GstspControlDialog.cpp StimDisplayWidget.cpp GstspClientDialog.cpp
HEADERS			=	HMM.h HMMTypes.h Port.h Stream.h Source.h Stim.h Counter.h GstspDialog.h StimFactory.h HabitStimFactory.h JsonStimFactory.h GstspControlDialog.h StimDisplayWidget.h GstspClientDialog.h
FORMS =	gstspcontroldialog.ui gstspclientdialog.ui
