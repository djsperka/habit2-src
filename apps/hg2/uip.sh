#!/bin/bash

AA=unknown
if [ $# -ne 1 ]; then
	echo "error need arg"
	exit -1
elif [ "$1x" == "releasex" ]; then
	AA=release
elif [ "$1x" == "debugx" ]; then
	AA="debug"
else
	echo unknown arg
	exit -1
fi

install_name_tool -add_rpath /Users/dan/install/lib ./${AA}/hg2.app/Contents/MacOS/hg2
install_name_tool -change libQt5GStreamerUi-1.0.0.dylib @rpath/libQt5GStreamerUi-1.0.0.dylib ./${AA}/hg2.app/Contents/MacOS/hg2
install_name_tool -change libQt5GStreamer-1.0.0.dylib @rpath/libQt5GStreamer-1.0.0.dylib ./${AA}/hg2.app/Contents/MacOS/hg2
install_name_tool -change libQt5GLib-2.0.0.dylib @rpath/libQt5GLib-2.0.0.dylib ./${AA}/hg2.app/Contents/MacOS/hg2
