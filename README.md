# habit2-src
Application for running habituation-type looking experiments

## Building
### Mac
```shell
cd distribution

# build x64
# The last arg "-i" gives the identity to pass to `codesign`; it should refer to an Apple Developer ID signing certificate stored in your Keychain.  
#
# If successful, the file **habit2-GGGGGEEEEE.dmg** is created, where *GGGGG* is the git tag or commit passed with "-g", and *EEEEE* is the arg passed with "-e". 
python3 build-distribution-package.py -q 5.15.2 -g v2.2.10 -d 2.2.10 -e _mac_x64 -i WWAFVH26T8

# When build finished, notarize. The --wait argument causes this command to wait until a response is received from Apple. 
xcrun notarytool submit habit2-v2.2.10_mac_x64.dmg  --keychain-profile "ALTOOL" --wait
Conducting pre-submission checks for habit2-v2.2.10_mac_x64.dmg and initiating connection to the Apple notary service...
Submission ID received
  id: 59c14c01-a866-4d90-bd9b-e9234fd18cb3
Upload progress: 100.00% (79.0 MB of 79.0 MB)   
Successfully uploaded file
  id: 59c14c01-a866-4d90-bd9b-e9234fd18cb3
  path: /Users/dan/git/habit2-src/distribution/habit2-v2.2.10_mac_x64.dmg
Waiting for processing to complete.
Current status: Accepted..................
Processing complete
  id: 59c14c01-a866-4d90-bd9b-e9234fd18cb3
  status: Accepted
  
# If accepted, then staple.
xcrun stapler staple habit2-v2.2.10_mac_x64.dmg 
Processing: /Users/dan/git/habit2-src/distribution/habit2-v2.2.10_mac_x64.dmg
Processing: /Users/dan/git/habit2-src/distribution/habit2-v2.2.10_mac_x64.dmg
The staple and validate action worked!



```

### Windows

Open Start>Visual Studio 2019>x64 Native Tools Command Prompt for VS2019
cd windows-setup

Edit windist.mak - the lines near top must be updated to reflect current status. GIT_TAG can be a commit or tag.

```shell
# remove build dir and artifacts
nmake /f windist.mak clean

# download src
nmake /f windist.mak DownloadSrc

# patch src. SED var must be set to point to a sed exe.
nmake /f windist.mak PatchSrc

# build
nmake /f windist.mak BuildSrc

# Get ready for packaging
nmake /f windist.mak StagingPrep

# Sign
nmake /f windist.mak SignExe

# Build installer
nmake /f windist.mak Package
```
