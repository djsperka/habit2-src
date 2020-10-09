#!/bin/sh

create-dmg \
  --volname "Habit2Installer" \
  --volicon "../apps/habit/habit2.icns" \
  --window-pos 200 120 \
  --window-size 800 400 \
  --icon-size 100 \
  --icon "habit2.app" 200 190 \
  --hide-extension "habit2.app" \
  --app-drop-link 600 185 \
  "Habit2-Installer.dmg" \
  "build/dist/"
 codesign -s "Developer ID Application: University of California- Davis (WWAFVH26T8)" Habit2-Installer.dmg