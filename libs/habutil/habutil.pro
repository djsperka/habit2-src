TEMPLATE = lib
CONFIG += qt staticlib debug_and_release
QMAKE_CXXFLAGS += -fvisibility=hidden
DESTDIR = ../../lib
HEADERS += HLook.h HLooker.h
SOURCES += HLook.cpp HLooker.cpp
