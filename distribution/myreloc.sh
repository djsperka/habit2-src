#!/bin/sh

# reloc.sh - relocate app for distribution
# usage: reloc.sh builddir bundle distdir
# - builddir e.g. release, bundle.app should live in this folder
# - bundle - bundle.app will be copied to distdir, and changes made to that copy. 
# - distdir - dir to do work. app bundle and libs copied here and modified in place. if this exists, will not run. Will not clobber.
#
# Assuming that exe name is same as bundle name, e.g. Contents/MacOS/bundle is the exe file

if [ $# -eq 4 ]
then
		
	BUILDDIR=$1	
	BUNDLE=$2
	LIBLIST=$3
	PKGLIST=""
	DISTDIR=$4
elif [ $# -eq 5 ]
then
	BUILDDIR=$1	
	BUNDLE=$2
	LIBLIST=$3
	PKGLIST=$4
	DISTDIR=$5
else
	echo "usage: reloc.sh BUILDDIR BUNDLE LIBLIST [PKGLIST] DISTDIR"
	exit -1
fi		
OSXRELOCATOR="$(which osxrelocator)"
MACDEPLOYQT="$(which macdeployqt)"

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

# mkdir Frameworks in app bundle
mkdir -p $DISTDIR/$BUNDLE.app/Contents/Frameworks/GStreamer.framework

# move GStreamer framework package - this package sets up the framework directory structure
# (note the -n)
tar -C /Library/Frameworks/GStreamer.framework/ -n -cf - `pkgutil --files org.freedesktop.gstreamer.x86_64.GStreamer` | tar -C $DISTDIR/$BUNDLE.app/Contents/Frameworks/GStreamer.framework -xf -

# if defined, copy files from PKGLIST
if [ ! -z "$PKGLIST" ]
then
	echo "getting files from pkglist file $PKGLIST"
	while IFS= read -r pkg
	do
  		echo "getting files from pkg: $pkg"
  		# | egrep '\.dylib$'
  		tar -C /Library/Frameworks/GStreamer.Framework/Versions/Current -cf - `pkgutil --files $pkg | grep -E 'dylib|plist|gst-plugin-scanner|gst-inspect-1.0|[.]so$'` | tar -C $DISTDIR/$BUNDLE.app/Contents/Frameworks/GStreamer.framework/Versions/Current -xf -
	done < "$PKGLIST"
fi
###################################

# Now copy plugin from $LIBLIST into plugin folder -- dumping into Frameworks folder (PKG_CONFIG_PATH must be set to th)
tar -cf - --files-from $LIBLIST | tar -C $DISTDIR/$BUNDLE.app/Contents/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0 -xf -

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
# I'm lazy so I'm doing it to the entire folder of plugins. It should only apply to 1 - mine. 
#$OSXRELOCATOR $DISTDIR/$BUNDLE.app/Contents/Frameworks/GStreamer.framework/Versions/Current/lib/gstreamer-1.0 /Library/Frameworks/GStreamer.framework/Versions/1.0 @rpath

# executable
$OSXRELOCATOR $DISTDIR/$BUNDLE.app/Contents/MacOS /Library/Frameworks/GStreamer.framework @executable_path/../Frameworks/GStreamer.framework

# relocate gstreamer libs
$OSXRELOCATOR $DISTDIR/$BUNDLE.app/Contents/Frameworks /Library/Frameworks/GStreamer.framework/Versions/1.0 @rpath -r

# gst-plugin-scanner has no rpath. create one: @executable_path/../..
# two dirs up from @executable_path because gstreamer distribution has : libexec/gstreamer-1.0/gst-plugin-scanner
install_name_tool -add_rpath @executable_path/../.. $DISTDIR/$BUNDLE.app/Contents/Frameworks/GStreamer.framework/Versions/1.0/libexec/gstreamer-1.0/gst-plugin-scanner


#
#
#
#
########################################################################################################
## relocate
########################################################################################################
#
## executable - make substitution for gstreamer/glib stuff, then add rpath for it. 
## Note: There is already an rpath for @executable_path/../Frameworks, this is for Qt, as they
## are linked via @rpath/QtGui.Framework/...
## add an rpath for gstreamer: @executable_path/../Frameworks/GStreamer.framework/Versions/1.0
#
#echo "relocate gstreamer plugins..."
#$OSXRELOCATOR $DISTDIR/$BUNDLE.app/Contents/MacOS /Library/Frameworks/GStreamer.framework/Versions/1.0 @rpath
#install_name_tool -add_rpath @executable_path/../Frameworks/GStreamer.framework/Versions/1.0 $DISTDIR/$BUNDLE.app/Contents/MacOS/$BUNDLE
#
## relocate gstreamer libs??? - this is not required
##$OSXRELOCATOR $DISTDIR/$BUNDLE.app/Contents/Frameworks/GStreamer.framework/Versions/Current/libexec /Library/Frameworks/GStreamer.framework/Versions/1.0 @rpath -r
#
## gst-plugin-scanner has no rpath. create one: @executable_path/../.. - that covers gstreamer/glib
## two dirs up from @executable_path because gstreamer distribution has : libexec/gstreamer-1.0/gst-plugin-scanner
#echo "fix rpath on gst-plugin-scanner"
#install_name_tool -add_rpath @executable_path/../.. $DISTDIR/$BUNDLE.app/Contents/Frameworks/GStreamer.framework/Versions/Current/libexec/gstreamer-1.0/gst-plugin-scanner
#install_name_tool -add_rpath @loader_path/../../../../.. $DISTDIR/$BUNDLE.app/Contents/Frameworks/GStreamer.framework/Versions/Current/libexec/gstreamer-1.0/gst-plugin-scanner
#
## same two rpaths for gst-inspect-1.0
#echo "fix rpath on gst-inspect-1.0"
#install_name_tool -add_rpath @executable_path/.. $DISTDIR/$BUNDLE.app/Contents/Frameworks/GStreamer.framework/Versions/Current/bin/gst-inspect-1.0
#install_name_tool -add_rpath @loader_path/../../../.. $DISTDIR/$BUNDLE.app/Contents/Frameworks/GStreamer.framework/Versions/Current/bin/gst-inspect-1.0
#
##install_name_tool -change /Library/Frameworks/GStreamer.framework @rpath/GStreamer.framework $DISTDIR/$BUNDLE.app/Contents/Frameworks/GStreamer.framework/Versions/Current/libexec/gstreamer-1.0/gst-plugin-scanner
#
## @loader_path/../../../../..
#


#######################################################################################################
# relocate - done
#######################################################################################################
echo "run macdeployqt..."
$MACDEPLOYQT $DISTDIR/$BUNDLE.app




