#!/bin/bash

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
COMPONENT_PLIST=habit2-component-${REV}.plist
pkgbuild --analyze --root root $COMPONENT_PLIST
echo "created component plist file $COMPONENT_PLIST"

# Now make the package
PACKAGE_FILE=habit2.pkg
rm -f $PACKAGE_FILE
pkgbuild --root root --component-plist $COMPONENT_PLIST --version $VERSION_FULL $PACKAGE_FILE

# The package name is embedded in the distribution file, so you either re-generate it 
# or do substitution here. TODO test re-generate, I think it puts a version in?
# Update - go bak to fixed file. Must change the generated package filename to be the same. 

DISTRIBUTION_PLIST=habit2-distribution.plist
#productbuild --synthesize --product habit2-requirements.plist --package $PACKAGE_FILE $DISTRIBUTION_PLIST

# create installer package
mkdir -p dist
rm -f dist/*
INSTALLER_PACKAGE_FILE=Habit2-$VERSION_FULL-mac-x64.pkg
productbuild --distribution $DISTRIBUTION_PLIST --resources . --package-path $PACKAGE_FILE  --sign "$INST_SIGNER" dist/$INSTALLER_PACKAGE_FILE

# create dmg installer
hdiutil create -volname "Habit2 Installer" -fs HFSX -srcfolder ./dist -ov Habit2-$VERSION_FULL-mac-x64.dmg






