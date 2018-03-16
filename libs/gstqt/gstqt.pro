TEMPLATE = lib
CONFIG += qt debug_and_release plugin

# Get gstreamer stuff from this file.
include(../../habit.pri)

QT += widgets
INCLUDEPATH += .
LIBS += -lgstvideo-1.0 -lgstbase-1.0
CONFIG(debug, debug|release) {
	DESTDIR = debug
} else {
	DESTDIR = release
}

DEFINES += \
			PACKAGE=\\\"qt-gstreamer\\\" \
			PACKAGE_NAME=\\\"QtGstreamer\\\" \
			PACKAGE_ORIGIN="\\\"http://gstreamer.freedesktop.org/\\\"" \
			PACKAGE_VERSION="\\\"1.2.0\\\"" \
			QWIDGETVIDEOSINK_NAME="qwidget5videosink" \
			QTVIDEOSINK_NAME="qt5videosink"

# Input
SOURCES +=	gstqwidgetvideosink.cpp \
			gstqtvideosinkbase.cpp \
			gstqtvideosink.cpp \
			gstqtvideosinkplugin.cpp \
			delegates/qwidgetvideosinkdelegate.cpp \
			delegates/qtvideosinkdelegate.cpp \
			delegates/basedelegate.cpp \
			utils/bufferformat.cpp \
			utils/utils.cpp \
			painters/genericsurfacepainter.cpp \
			gstqtvideosinkmarshal.c
HEADERS +=	gstqwidgetvideosink.h \
			gstqtvideosinkbase.h \
			gstqtvideosinkplugin.h \
			delegates/qwidgetvideosinkdelegate.h \
			delegates/qtvideosinkdelegate.h \
			delegates/basedelegate.h \
			utils/bufferformat.h \
			utils/utils.h \
			painters/abstractsurfacepainter.h \
			painters/genericsurfacepainter.h \
			gstqtvideosinkmarshal.h

