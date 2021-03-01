#!/bin/sh


# djs taken from 
#
# https://developer.apple.com/forums/thread/130855
#

#
# might be useful in testing:
# tccutil reset All edu.ucdavis.infantcognitionlab.habit2
# rm ~/.cache/gstreamer-1.0/registry.x86_64.bin
#

# Fail if any command fails.

set -e

# Check and unpack the arguments.

if [ $# -ne 2 ]
then
	echo "usage: sign.sh /path/to/folder/containing/app SIGID" > /dev/stderr
	echo "$#" > /dev/stderr
    exit 1
fi
FOLDER="$1"
SIGID="$2"

# "Developer ID Application: University of California- Davis (WWAFVH26T8)"

# Establish a work directory, create a disk image root directory within 
# that, and then copy the app there.
#
# Note we use `-R`, not `-r`, to preserve symlinks.

WORKDIR=${FOLDER}/"signed-`date '+%Y-%m-%d_%H.%M.%S'`"
WORKDIR=${FOLDER}/signed
APP="${WORKDIR}/habit2.app"
DMG="${WORKDIR}/habit2.dmg"


echo "create work folder ${WORKDIR}"
mkdir -p "${WORKDIR}"

echo "copy habit2.app to ${WORKDIR}"
tar -C ${FOLDER} -cf - habit2.app | tar -C ${WORKDIR} -xf -

# When you use `-f` to replace a signature, `codesign` prints `replacing 
# existing signature`.  There's no option to suppress that.  The message 
# goes to `stderr` so you don't want to redirect it to `/dev/null` because 
# there might be other interesting stuff logged to `stderr`.  One way to 
# prevent it is to remove the signature beforehand, as shown by the 
# following lines.  It does slow things down a bunch though, so I've made 
# it easy to disable them.

if false
then
    codesign --remove-signature "${APP}"
    codesign --remove-signature "${APP}/Contents/PlugIns/QShareExtension.appex"
    codesign --remove-signature "${APP}/Contents/Frameworks/QCore.framework"
    codesign --remove-signature "${APP}/Contents/Frameworks/QCore.framework/Versions/A/Helpers/QCoreTool"
fi

# Create various entitlement files from 'here' documents.
# djs not sandboxing the app
echo "create entitlements file app.entitlements ..."
cat > "${FOLDER}/app.entitlements" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>com.apple.security.cs.allow-unsigned-executable-memory</key>
    <true/>
</dict>
</plist>
EOF

#
#echo "create entitlements file tool.entitlements ..."
#cat > "${WORKDIR}/tool.entitlements" <<EOF
#<?xml version="1.0" encoding="UTF-8"?>
#<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
#<plist version="1.0">
#<dict>
#    <key>com.apple.security.app-sandbox</key>
#    <true/>
#    <key>com.apple.security.inherit</key>
#    <true/>
#</dict>
#</plist>
#EOF

# Sign the app from the inside out.
#
# Notes:
#
# * The tool is within the framework, so we have to sign that before the 
#   framework.
#
# * The app and the appex depend on the framework, so we have to sign the 
#   framework before of those.
# 
# * The appex is within the app, so we have to sign that before the app.
#
# * The tool is not bundled, thus doesn't have an `Info.plist`, and thus 
#   you have to explicitly set a code signing identifier.
# 
# * The tool, appex and app are all executables, and thus need to the 
#   hardened runtime flag.
# 
# * The tool, appex and app all need unique entitlements.

#codesign -s "Developer ID Application" -f --timestamp -i com.example.apple-samplecode.QShare.QCoreTool -o runtime --entitlements "${WORKDIR}/tool.entitlements"  "${APP}/Contents/Frameworks/QCore.framework/Versions/A/Helpers/QCoreTool"
#codesign -s "Developer ID Application" -f --timestamp                                                                                                            "${APP}/Contents/Frameworks/QCore.framework"
#codesign -s "Developer ID Application" -f --timestamp                                                  -o runtime --entitlements "${WORKDIR}/appex.entitlements" "${APP}/Contents/PlugIns/QShareExtension.appex"
#codesign -s "Developer ID Application" -f --timestamp                                                  -o runtime --entitlements "${WORKDIR}/app.entitlements"   "${APP}"


# djs
#
# app depends on all frameworks
# tool (gst-plugin-scanner and gst-inspect-1.0) depend on GStreamer.framework
# BOTH of these need hardened runtime flag
# gst-plugin-scanner and gst-inspect-1.0 are not bundled, must explicitly set code signing 
# identifier (-i ...)
#
# plugin depends on GStreamer framework and Qt framework, so must sign the frameworks before plugin?
# First try signing Qt frameworks first - then GStreamer (with plugin inside GStreamer framework)
#


# gst-plugin-scanner
echo "sign gst-plugin-scanner..."
codesign 	-s  ${SIGID} \
			-f --timestamp \
			-i "org.freedesktop.gstreamer.gst-plugin-scanner" \
			-o runtime \
			"${APP}/Contents/Macos/gst-plugin-scanner"

# gst-inspect-1.0
echo "sign gst-inspect-1.0..."
codesign 	-s ${SIGID} \
			-f --timestamp \
			-i "org.freedesktop.gstreamer.gst-inspect-1.0" \
			-o runtime \
			"${APP}/Contents/MacOS/gst-inspect-1.0"

# plugins
echo "sign plugins..."
for ifile in `find ${APP}/Contents/PlugIns/ -name '*.dylib'`; do
	echo "sign plugin ${ifile}"
	codesign 	-s ${SIGID} \
				-f --timestamp \
				"${ifile}"
done

# frameworks

echo "sign GStreamer dylibs and so libs..."
for ifile in `find ${APP}/Contents/Frameworks/lib -name '*.dylib' -o -name '*.so'`; do
	echo "sign gstreamer dylib ${ifile}"
	codesign 	-s ${SIGID} \
				-f --timestamp \
				"${ifile}"
done


echo "sign QtCore.framework..."
codesign 	-s ${SIGID} \
			-f --timestamp \
			"${APP}/Contents/Frameworks/QtCore.framework"

echo "sign QtDBus.framework..."
codesign 	-s ${SIGID} \
			-f --timestamp \
			"${APP}/Contents/Frameworks/QtDBus.framework"

echo "sign QtGui.framework..."
codesign 	-s ${SIGID} \
			-f --timestamp \
			"${APP}/Contents/Frameworks/QtGui.framework"
			
echo "sign QtPrintSupport.framework..."
codesign 	-s ${SIGID} \
			-f --timestamp \
			"${APP}/Contents/Frameworks/QtPrintSupport.framework"

echo "sign QtSql.framework..."
codesign 	-s ${SIGID} \
			-f --timestamp \
			"${APP}/Contents/Frameworks/QtSql.framework"
			
echo "sign QtSvg.framework..."
codesign 	-s ${SIGID} \
			-f --timestamp \
			"${APP}/Contents/Frameworks/QtSvg.framework"

echo "sign QtWidgets.framework..."
codesign 	-s ${SIGID} \
			-f --timestamp \
			"${APP}/Contents/Frameworks/QtWidgets.framework"

# app

echo "sign habit2.app..." 
codesign 	-s ${SIGID} \
			-f --timestamp \
			-o runtime \
			--entitlements "${FOLDER}/app.entitlements" \
			"${APP}"
			
#			--entitlements "${WORKDIR}/app.entitlements"
			



# Create a disk image from our disk image root directory.

#hdiutil create -srcFolder "${DMGROOT}" -quiet -o "${DMG}"

# Sign that.

#codesign -s "Developer ID Application" --timestamp -i com.example.apple-samplecode.QShare.DiskImage "${DMG}"

#echo "${DMG}"