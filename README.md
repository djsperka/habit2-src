# habit2-src
Application for running habituation-type looking experiments

## Building
### Mac
```shell
cd distribution
# build x64 for mojave
python3 build-distribution-package.py -q 5.12.1 -g v2.2.7.1 -d 2.2.7.1 -s 2.2.7.1 -e _mac_x64 -t "Habit 2.2.7.1"
# build x64 for sierra
python3 build-distribution-package.py -q 5.9.1 -g v2.2.7.1 -d 2.2.7.1 -s 2.2.7.1 -e _mac_sierra_x64 -t "Habit 2.2.7.1"
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
