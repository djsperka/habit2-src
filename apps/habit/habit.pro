# This is a qmake project file, provided as an example on how to use qmake with QtGStreamer.

TEMPLATE = app
TARGET = habit2

# need this for boost?
#INCLUDEPATH += 	"/usr/local/include" 

# Tell qmake to use pkg-config to find gstreamer.
CONFIG += debug_and_release c++11

# need sql libs and print support
QT += sql printsupport

# Get gstreamer stuff from this file.
include(../../habit2.pri)

# Now tell qmake to link to gstreamer and also use its include path and Cflags.
#PKGCONFIG += gstreamer-1.0 gstreamer-plugins-base-1.0
# gstreamer-plugins-bad-1.0
QT += widgets

# Recommended, to avoid possible issues with the "emit" keyword
# You can otherwise also define QT_NO_EMIT, but notice that this is not a documented Qt macro.
DEFINES += QT_NO_KEYWORDS

CONFIG(debug, debug|release) {
	DESTDIR = debug
	!win32 {
		LIBS += -L../../libs/habutil/debug -lhabutil
		PRE_TARGETDEPS += ../../libs/habutil/debug/libhabutil.a
	} else {
		LIBS += -L../../libs/habutil/debug -lhabutil
		PRE_TARGETDEPS += ../../libs/habutil/debug/habutil.lib
	}
	DEFINES += HABIT_DEBUG
} else {
	DESTDIR = release
	!win32 {
		LIBS += -L../../libs/habutil/release -lhabutil
		PRE_TARGETDEPS += ../../libs/habutil/release/libhabutil.a
	} else {
		LIBS += -L../../libs/habutil/release -lhabutil
		PRE_TARGETDEPS += ../../libs/habutil/release/habutil.lib
	}
	DEFINES += HABIT_RELEASE
}

macx {
	ICON = ./habit2.icns
	QMAKE_INFO_PLIST = habit.plist
}
win32 {
	RC_ICONS = ../../icons/kids-icon.ico
}

INCLUDEPATH += ../../libs/habutil
INCLUDEPATH += ../../libs/habutil/habit
DEPENDPATH += ../../libs/habutil ../../libs/habutil/habit
SOURCES			=	main.cpp 
HEADERS			=	version.h


#PKGROOT = $$_PRO_FILE_PWD_/../../distribution/pkgroot
PKGROOT = $$_PRO_FILE_PWD_/../../distribution/root/Applications
message(pkg root value $$PKGROOT)
message(TARGET value $$TARGET)
TOOLS = $$_PRO_FILE_PWD_/../../tools
message(tools value $$TOOLS)
HABIT_PLUGIN = $$_PRO_FILE_PWD_/../../libs/gstqt/release/libgstqt.dylib
message(HABIT_PLUGIN $$HABIT_PLUGIN)
message(QT bins $$[QT_INSTALL_BINS])

# call this one to generate a full release .pkg
build_release_pkg.target = .build_release_pkg
build_release_pkg.commands = touch .build_release_pkg && echo "Built release package"
build_release_pkg.depends = relocate_app

# perform relocation tricks on distribution app
relocate_app.target = .relocate_app
relocate_app.commands = PATH=$$[QT_INSTALL_BINS]:/usr/local/bin:$(PATH) $${TOOLS}/relocate_habit_for_distribution $${PKGROOT}/habit2.app && touch .relocate_app
relocate_app.depends = copy_gstreamer copy_plugin

# copy gstreamer to dist app folder
copy_gstreamer.target = .copy_gstreamer
copy_gstreamer.commands = echo "Copying GStreamer framework to $$PKGROOT/habit2.app/Contents/Frameworks" && $$TOOLS/copy_gstreamer_framework $$PKGROOT/habit2.app/Contents/Frameworks && touch .copy_gstreamer && echo "Copying GStreamer framework to $$PKGROOT/habit2.app/Contents/Frameworks - Done."
copy_gstreamer.depends = copy_app

# copy qt plugin to dist app folder
copy_plugin.target = .copy_plugin
copy_plugin.commands = cp $$HABIT_PLUGIN $$PKGROOT/habit2.app/Contents/Frameworks/GStreamer.framework/Versions/Current/lib/gstreamer-1.0 && touch .copy_plugin
copy_plugin.depends = copy_app

# make .copy_app will copy current "release" build to pkgroot dir
copy_app.target = .copy_app
copy_app.commands = echo "Copy app to "$$PKGROOT"..." && (tar -C release -cf - habit2.app | tar -C $$PKGROOT -xf -) && touch .copy_app && echo "Copy app to "$$PKGROOT"...Done."
copy_app.depends = clean release clean_pkgroot

# clean pkgroot of any previously used app. 
clean_pkgroot.target = .clean_pkgroot
clean_pkgroot.commands = echo "Cleaning pkgroot $$PKGROOT..." && rm -rf $${PKGROOT}/habit2.app && touch .clean_pkgroot && echo "Cleaning pkgroot $$PKGROOT...Done."
clean_pkgroot.depends = FORCE
QMAKE_EXTRA_TARGETS += build_release_pkg copy_app clean_pkgroot copy_plugin copy_gstreamer relocate_app
