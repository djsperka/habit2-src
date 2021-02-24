#!/bin/sh

# reloc.sh - relocate app for distribution
# usage: reloc.sh builddir bundle distdir
# - builddir e.g. release, bundle.app should live in this folder
# - bundle - bundle.app will be copied to distdir, and changes made to that copy. 
# - distdir - dir to do work. app bundle and libs copied here and modified in place. if this exists, will not run. Will not clobber.
#
# Assuming that exe name is same as bundle name, e.g. Contents/MacOS/bundle is the exe file

if [ $# -eq 5 ]
then
	BUILDDIR=$1	
	BUNDLE=$2
	LIBLIST=$3
	PKGLIST=$4
	DISTDIR=$5
#	MOVLIST=$6
else
	echo "usage: reloc.sh BUILDDIR BUNDLE LIBLIST PKGLIST MOVLIST DISTDIR"
	exit -1
fi		
OSXRELOCATOR="$(which osxrelocator)"
#MACDEPLOYQT="$(which macdeployqt)"

# if DISTDIR exists, make sure bundle isn't in it.
if [ -e $DISTDIR ]
then
	echo "DISTDIR already exists ($DISTDIR). Remove please."
	exit -1
fi

mkdir -p $DISTDIR
if [ $? -ne 0 ]
then
	echo "Cannot create $DISTDIR"
	exit -1
fi


###################################
# move/copy files to dist directory
###################################


# copy bundle to distdir
tar -C $BUILDDIR -cf - $BUNDLE.app | tar -C $DISTDIR -xf -

# create frameworks dir
mkdir -p $DISTDIR/$BUNDLE.app/Contents/Frameworks

# if defined, copy files from PKGLIST
if [ ! -z "$PKGLIST" ]
then
	echo "getting files from pkglist file $PKGLIST"
	while IFS= read -r pkg
	do
  		echo "getting files from pkg: $pkg"
  		# | egrep '\.dylib$'
  		tar -C /Library/Frameworks/GStreamer.Framework/Versions/Current -cf - `pkgutil --files $pkg | grep -E 'dylib|plist|gst-plugin-scanner|gst-inspect-1.0|[.]so$'` | tar -C $DISTDIR/$BUNDLE.app/Contents/Frameworks -xf -
	done < "$PKGLIST"
fi
###################################

# Now copy plugin from $LIBLIST into plugin folder -- dumping into Frameworks folder (PKG_CONFIG_PATH must be set to th)
#tar -cf - --files-from $LIBLIST | tar -C $DISTDIR/$BUNDLE.app/Contents/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0 -xf -

# if defined, copy files from LIBLIST
# If lib  has path, tar -C to that
if [ ! -z "$LIBLIST" ]
then
	echo "getting files from liblist file $LIBLIST"
	while IFS= read -r lib
	do
		tar -C `dirname $lib` -cf - `basename $lib` | tar -C $DISTDIR/$BUNDLE.app/Contents/Frameworks/lib/gstreamer-1.0/ -xf -
done < "$LIBLIST"
fi

# move gst-plugin-scanner and gst-inspect-1.0
mv $DISTDIR/$BUNDLE.app/Contents/Frameworks/libexec/gstreamer-1.0/gst-plugin-scanner $DISTDIR/$BUNDLE.app/Contents/MacOS
mv $DISTDIR/$BUNDLE.app/Contents/Frameworks/bin/gst-inspect-1.0 $DISTDIR/$BUNDLE.app/Contents/MacOS
rm -rf $DISTDIR/$BUNDLE.app/Contents/Frameworks/libexec
rm -rf $DISTDIR/$BUNDLE.app/Contents/Frameworks/bin




# relocation on the plugin is required. 
# the plugin build (as of 8/26/20) leaves gstreamer libs linked with full paths, and 
# Qt libs linked via @rpath/QtGui.framework/...

# Note: official plugins are linked thusly, with gstreamer libs linked via @rpath/lib/libgst....dylib 
# Note also: all gstreamer libs in Versions/1.0/lib are linked that way as well (same for glib)
# Any app that uses these libs must have rpath that includes the app bundle's 
# Frameworks/GStreamer.framework/Versions/1.0 path - if so, all the gstreamer/glib stuff will be fine.

# change plugin to be like regular plugins, so change
#
# /Library/Frameworks/GStreamer.framework/Versions/1.0
#
# to
#
# @rpath
#
# executable files
$OSXRELOCATOR $DISTDIR/$BUNDLE.app/Contents/MacOS /Library/Frameworks/GStreamer.framework/Versions/1.0 @rpath
install_name_tool -add_rpath @executable_path/../Frameworks $DISTDIR/$BUNDLE.app/Contents/MacOS/habit2
install_name_tool -add_rpath @executable_path/../Frameworks $DISTDIR/$BUNDLE.app/Contents/MacOS/gst-plugin-scanner
install_name_tool -add_rpath @executable_path/../Frameworks $DISTDIR/$BUNDLE.app/Contents/MacOS/gst-inspect-1.0

# relocate gstreamer libs
$OSXRELOCATOR $DISTDIR/$BUNDLE.app/Contents/Frameworks /Library/Frameworks/GStreamer.framework/Versions/1.0 @rpath -r


# last step - rename "gstreamer-1.0" to "gstreamer-1_0"
mv $DISTDIR/$BUNDLE.app/Contents/Frameworks/lib/gstreamer-1.0 $DISTDIR/$BUNDLE.app/Contents/Frameworks/lib/gstreamer-1_0

#######################################################################################################
# relocate - done
#######################################################################################################




