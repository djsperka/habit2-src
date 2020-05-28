#!/bin/sh

GST_PLUGIN_PATH=/Users/dan/git/habit2-src/libs/gstqt/release \
GST_DEBUG=3 \
GST_DEBUG_DUMP_DOT_DIR=./ \
./release/gstsp.app/Contents/MacOS/gstsp -N \
-N -s 127.0.0.1 -p 5255 -l /Users/dan/git/habit2-src/apps/gstsp/first.json -r /Users/dan/git/habit2-src/distribution/stim/examples

