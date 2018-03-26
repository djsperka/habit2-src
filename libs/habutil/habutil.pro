message("inside habutil.pro")

TEMPLATE = lib
CONFIG += qt staticlib debug_and_release
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets printsupport
}

QMAKE_CXXFLAGS += -fvisibility=hidden
INCLUDEPATH += . ./habit ./wizards ./hgst
CONFIG(debug, debug|release) {
	DESTDIR = debug
} else {
	DESTDIR = release
}

message("reading habit2.pri")
# Get gstreamer stuff from this file.
include(../../habit2.pri)

QT += widgets

message("reading habutil.pri")
include(habutil.pri)