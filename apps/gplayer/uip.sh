#!/bin/bash

install_name_tool -add_rpath /Users/dan/install/lib ./player.app/Contents/MacOS/player
install_name_tool -change libQt5GStreamerUi-1.0.0.dylib @rpath/libQt5GStreamerUi-1.0.0.dylib ./player.app/Contents/MacOS/player
install_name_tool -change libQt5GStreamer-1.0.0.dylib @rpath/libQt5GStreamer-1.0.0.dylib ./player.app/Contents/MacOS/player
install_name_tool -change libQt5GLib-2.0.0.dylib @rpath/libQt5GLib-2.0.0.dylib ./player.app/Contents/MacOS/player
