# On the mac, use pkgconfig for gstreamer stuff
# Its broken there, for some reason when you run pkg-config from within
# eclipse it does not pick up PKG_CONFIG_PATH, and I have not found a 
# workaround other than to generate makefiles from the command line. 
# On Windows its worse, I have no idea, I just put the libs and include 
# files (taken from the *.pc files themselves) in manually. 
# On the mac, I also do /usr/local/include because I think its needed (by what?)

!win32 {
     GFLAGS = $$system(pkg-config --libs gstreamer-1.0)
     GFLAGS += -lgstvideo-1.0
     LIBS += $$GFLAGS

     IFLAGS = $$system(pkg-config --cflags gstreamer-1.0)
     QMAKE_CXXFLAGS += $$IFLAGS


 #   CONFIG += link_pkgconfig c++11
 #   PKGCONFIG += gstreamer-1.0
	#QMAKE_MACOSX_DEPLOYMENT_TARGET=10.13
 #   INCLUDEPATH += \
 #                   /usr/local/include \
 #                   /Users/dan/git/gst-plugins-base \
 #                   /Users/dan/git/gst-plugins-good
} else {
	# MIN DEPLOYMENT TARGET is 10.9 - Mavericks
    CONFIG += c++11
    LIBS += -Lc:/gstreamer/1.0/x86_64/lib -lgstreamer-1.0 -lglib-2.0 -lgobject-2.0 -lintl 
#    LIBS += -lgstreamer-1.0 -lglib-2.0 -lintl 
    INCLUDEPATH +=  \
                    c:/gstreamer/1.0/x86_64/include/glib-2.0 \
                    c:/gstreamer/1.0/x86_64/lib/glib-2.0/include \
                    c:/gstreamer/1.0/x86_64/include/gstreamer-1.0
}
