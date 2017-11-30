# This is a qmake project file, provided as an example on how to use qmake with QtGStreamer.

TEMPLATE = app
TARGET = habit

target.path = $$_PRO_FILE_PWD_/../../distribution/pkgroot
INSTALLS += target

# need this for boost?
INCLUDEPATH += 	"/usr/local/include" 

# produce nice compilation output
#CONFIG += silent

# Tell qmake to use pkg-config to find gstreamer.
CONFIG += debug_and_release link_pkgconfig c++11

# need sql libs and print support
QT += sql printsupport

#QMAKE_FLAGS += "-Wl,-rpath,\'/Users/dan/install/lib' -Wl,-rpath,\'\$$ORIGIN\'"

# Now tell qmake to link to gstreamer and also use its include path and Cflags.
PKGCONFIG += gstreamer-1.0 gstreamer-plugins-base-1.0
# gstreamer-plugins-bad-1.0
QT += widgets

# Recommended if you are using g++ 4.5 or later. Must be removed for other compilers.
#QMAKE_CXXFLAGS += -std=c++0x

# Recommended, to avoid possible issues with the "emit" keyword
# You can otherwise also define QT_NO_EMIT, but notice that this is not a documented Qt macro.
DEFINES += QT_NO_KEYWORDS



CONFIG(debug, debug|release) {
	DESTDIR = debug
	LIBS += -L../../libs/habutil/debug -lhabutil
	PRE_TARGETDEPS += ../../libs/habutil/debug/libhabutil.a
#	QMAKE_POST_LINK = ./uip.sh debug
} else {
	DESTDIR = release
	LIBS += -L../../libs/habutil/release -lhabutil
	PRE_TARGETDEPS += ../../libs/habutil/release/libhabutil.a
#	QMAKE_POST_LINK = ./uip.sh release
}

ICON = ./habiticon.icns
QMAKE_INFO_PLIST = habit.plist

INCLUDEPATH += ../../libs/habutil
INCLUDEPATH += ../../libs/habutil/habit
DEPENDPATH += ../../libs/habutil ../../libs/habutil/habit
SOURCES			=	main.cpp 
HEADERS			=	version.h
					
