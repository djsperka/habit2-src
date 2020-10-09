ditto -c -k --keepParent ./Habit2-Installer.dmg h228test.zip

xcrun altool --notarize-app --primary-bundle-id "habit2-2.2.8-rc1" -u djsperka@ucdavis.edu -p @keychain:NOTARIZE --file h228test.zip

xcrun altool --notarization-info 0ba9c4e7-dec9-4819-a934-383d0c08169c -u djsperka@ucdavis.edu -p @keychain:NOTARIZE

xcrun stapler staple Habit2-Installer.dmg 