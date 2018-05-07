# This is a qmake project file, provided as an example on how to use qmake with QtGStreamer.

TEMPLATE = app
TARGET = gplayer

# need this for boost?
INCLUDEPATH += "/usr/local/include"

# Tell qmake to use pkg-config to find QtGStreamer.
CONFIG += qt debug_and_release link_pkgconfig c++11

QMAKE_FLAGS += "-Wl,-rpath,\'/Users/dan/install/lib' -Wl,-rpath,\'\$$ORIGIN\'"

# Now tell qmake to link to QtGStreamer and also use its include path and Cflags.
contains(QT_VERSION, ^4\\..*) {
  PKGCONFIG += QtGStreamer-1.0 QtGStreamerUi-1.0
}
contains(QT_VERSION, ^5\\..*) {
  PKGCONFIG += Qt5GStreamer-1.0 Qt5GStreamerUi-1.0
  QT += widgets
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

INCLUDEPATH += ../../libs/habutil 
INCLUDEPATH += ../../libs/habutil/habit
DEPENDPATH += ../../libs/habutil ../../libs/habutil/habit 



# Input
HEADERS = mediaapp.h player.h
SOURCES = main.cpp mediaapp.cpp player.cpp

#message(PKG_CONFIG_PATH when qmake is run $$PKG_CONFIG_PATH)
#message(PKG_CONFIG_LIBDIR when qmake is run $$PKG_CONFIG_LIBDIR)