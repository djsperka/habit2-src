# On the mac, use pkgconfig for gstreamer stuff
# Its broken there, for some reason when you run pkg-config from within
# eclipse it does not pick up PKG_CONFIG_PATH, and I have not found a 
# workaround other than to generate makefiles from the command line. 
# On Windows its worse, I have no idea, I just put the libs and include 
# files (taken from the *.pc files themselves) in manually. 
# On the mac, I also do /usr/local/include because I think its needed (by what?)

!win32 {
    CONFIG += link_pkgconfig c++11
    PKGCONFIG += gstreamer-1.0 gst-validate-1.0
 #   INCLUDEPATH += \
 #                   /usr/local/include \
 #                   /Users/dan/git/gst-plugins-base \
 #                   /Users/dan/git/gst-plugins-good
} else {
	# MIN DEPLOYMENT TARGET is 10.9 - Mavericks
	# QMAKE_MACOSX_DEPLOYMENT_TARGET=10.11
	
	#set this to get correct sdk min value in exe
	#QMAKE_LFLAGS += -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk

    CONFIG += c++11
	
	# djs change to backslashes for windows
	# change additions to INCLUDEPATH to single line
    LIBS += -Lc:\gstreamer\1.0\msvc_x86_64\lib -lgstvalidate-1.0 -lgstreamer-1.0 -lglib-2.0 -lgobject-2.0 -lintl
    INCLUDEPATH += c:\gstreamer\1.0\msvc_x86_64\include\glib-2.0
    INCLUDEPATH += c:\gstreamer\1.0\msvc_x86_64\lib\glib-2.0\include
    INCLUDEPATH += c:\gstreamer\1.0\msvc_x86_64\include\gstreamer-1.0
}
