#!/bin/bash

# BUILD_DIR is the folder where we do the building. Generated files live there.
BUILD_DIR=./
mkdir -p $BUILD_DIR

# set these for signing the app and installer
APP_SIGNER="3rd Party Mac Developer Application: University of California- Davis (WWAFVH26T8)"
INST_SIGNER="3rd Party Mac Developer Installer: University of California- Davis (WWAFVH26T8)"

# get svn revision
# ASSUME we are inside of project. TODO: Can add project name to command?
REV=`svn info | grep "Revision:" | cut -d : -f 2 | tr -d '[:space:]'`
echo "at SVN revision $REV"

# version
# TODO VERSION should come from elsewhere
VERSION=2.2.1
VERSION_FULL=${VERSION}.${REV}

# sign
# ASSUME app has been relocated & prepped for shipment (make .build_release_pkg)
codesign -v -f --deep --sign "$APP_SIGNER" root/Applications/habit2.app

# pkgutil takes a look at the folder to get component property list for package
COMPONENT_PLIST=${BUILD_DIR}/habit2-component-${REV}.plist
pkgbuild --analyze --identifier edu.ucdavis.infantcognitionlab.habit2 --root root $COMPONENT_PLIST
echo "created component plist file $COMPONENT_PLIST"

# Now make the package
PACKAGE_FILE=${BUILD_DIR}/habit2.pkg
rm -f $PACKAGE_FILE
pkgbuild --root root --component-plist $COMPONENT_PLIST --version $VERSION_FULL $PACKAGE_FILE

# The package name is embedded in the distribution file, so you either re-generate it 
# or do substitution here. TODO test re-generate, I think it puts a version in?
# Update - go bak to fixed file. Must change the generated package filename to be the same. 

DISTRIBUTION_PLIST=habit2-distribution.plist
#productbuild --synthesize --product habit2-requirements.plist --package $PACKAGE_FILE $DISTRIBUTION_PLIST

# create installer package
DMG_DIR=${BUILD_DIR}/dist
mkdir -p $DMG_DIR
rm -f ${DMG_DIR}/*.pkg
INSTALLER_PACKAGE_FILE=${DMG_DIR}/Habit2-$VERSION_FULL-mac-x64.pkg
productbuild --version $VERSION_FULL --distribution $DISTRIBUTION_PLIST --resources files --package-path $PACKAGE_FILE  --sign "$INST_SIGNER" $INSTALLER_PACKAGE_FILE

# create dmg installer
INSTALLER_DMG_FILE=${BUILD_DIR}/Habit2-${VERSION_FULL}-mac-x64.dmg
hdiutil create -volname "Habit2 Installer" -fs HFSX -srcfolder $DMG_DIR -ov $INSTALLER_DMG_FILE






