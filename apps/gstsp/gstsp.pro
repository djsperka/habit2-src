TARGET = gstsp
TEMPLATE = app
QMAKE_CXXFLAGS += -fvisibility=hidden

QT += sql widgets printsupport multimedia
CONFIG += qt debug_and_release c++11

# Get gstreamer stuff from this file.
include(../../habit2.pri)

# Info.plist file on Mac


# get out of the pkgconfig business for this application.
# here the problem is lib ordering -- qmake puts pkgconfig stuff at the end of the list, 
# which causes /usr/local/lib to satisfy some dependencies (which is bad for me)

macx {
    QMAKE_INFO_PLIST = gstsp.plist
    
 #   CONFIG -= link_pkgconfig
    
 #   QTCFLAGS = $$system("PKG_CONFIG_PATH/Library/Frameworks/GStreamer.framework/Versions/1.0/lib/pkgconfig /usr/local/bin/pkg-config --cflags gstreamer-1.0")
 #   message($$QTCFLAGS)
}


CONFIG(debug, debug|release) {
	DESTDIR = debug
	!win32 {
		CONFIG -= link_pkgconfig
		INCLUDEPATH += 	/Library/Frameworks/GStreamer.framework/Versions/1.0/include/gstreamer-1.0 \
						/Library/Frameworks/GStreamer.framework/Versions/1.0/include \
						/Library/Frameworks/GStreamer.framework/Versions/1.0/include/glib-2.0 \
						/Library/Frameworks/GStreamer.framework/Versions/1.0/lib/glib-2.0/include \
						/Library/Frameworks/GStreamer.framework/Versions/1.0/include/json-glib-1.0 \
						/Library/Frameworks/GStreamer.framework/Versions/1.0/include/libsoup-2.4 \
						/Library/Frameworks/GStreamer.framework/Versions/1.0/include/libxml2 
						
		LIBS += -L/Library/Frameworks/GStreamer.framework/Versions/1.0/lib -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0 -lintl
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
		CONFIG -= link_pkgconfig
		INCLUDEPATH += 	/Library/Frameworks/GStreamer.framework/Versions/1.0/include/gstreamer-1.0 \
						/Library/Frameworks/GStreamer.framework/Versions/1.0/include \
						/Library/Frameworks/GStreamer.framework/Versions/1.0/include/glib-2.0 \
						/Library/Frameworks/GStreamer.framework/Versions/1.0/lib/glib-2.0/include \
						/Library/Frameworks/GStreamer.framework/Versions/1.0/include/json-glib-1.0 \
						/Library/Frameworks/GStreamer.framework/Versions/1.0/include/libsoup-2.4 \
						/Library/Frameworks/GStreamer.framework/Versions/1.0/include/libxml2 
						
		LIBS += -L/Library/Frameworks/GStreamer.framework/Versions/1.0/lib -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0 -lintl
		LIBS += -L/usr/local/lib -lboost_filesystem -L../../libs/habutil/release -lhabutil
		PRE_TARGETDEPS += ../../libs/habutil/release/libhabutil.a
	} else {
		LIBS += -L../../libs/habutil/release -lhabutil
		PRE_TARGETDEPS += ../../libs/habutil/release/habutil.lib
	}
	DEFINES += HABIT_RELEASE

}



#mytarget.target = $$DESTDIR/$$TARGET.app/Stimuli
mytarget.target = stimuli
mytarget.commands = ./copy_stimuli.sh $$DESTDIR
mytarget.CONFIG = recursive
QMAKE_EXTRA_TARGETS += mytarget

INCLUDEPATH += 	../../libs/habutil \
				../../libs/habutil/habit \
				../../libs/habutil/hgst \
				/usr/local/include

DEPENDPATH += ../../libs/habutil ../../libs/habutil/habit ../../libs/habutil/hgst 
SOURCES			=	gstsp.cpp  HMM.cpp Port.cpp Source.cpp Stim.cpp Counter.cpp GstspDialog.cpp HabitStimFactory.cpp JsonStimFactory.cpp GstspControlDialog.cpp StimDisplayWidget.cpp GstspClientDialog.cpp webrtc_helper.cpp
HEADERS			=	HMM.h HMMTypes.h Port.h Stream.h Source.h Stim.h Counter.h GstspDialog.h StimFactory.h HabitStimFactory.h JsonStimFactory.h GstspControlDialog.h StimDisplayWidget.h GstspClientDialog.h
FORMS =	gstspcontroldialog.ui gstspclientdialog.ui

 
 # mytarget.depends = mytarget2
 
 
 