# env can contain these, else set them here
GIT_TAG=v2.2.5-rc4
DIST_VERSION=2.2.5
EXTRA_LABEL=_win_x64
QT_BIN=C:\Qt\5.10.1\msvc2017_64\bin

# qmake exe based on QT_BIN - this sets the qt version to use in the dist
QMAKE=$(QT_BIN)\qmake.exe

# export key from certmgr, set filename and password here
# delete file when build is done!
PFXFILE=ucd.pfx
PFXKEY=horseyface

# Note: $(MAKEDIR) is the location of this makefile. 
BUILDDIR=$(MAKEDIR)\build
STAGINGDIR=$(BUILDDIR)\staging
OBJDIR=$(BUILDDIR)\obj
GENDIR=$(BUILDDIR)\generated
SRCDIR=$(BUILDDIR)\src

# these are for stuff within the habit2-src tree
PROJROOT=$(MAKEDIR)\..
DISTSTIMDIR=$(SRCDIR)\distribution\stim
DISTEXEDIR=$(SRCDIR)\apps\habit\release

# wix plugin we use
WIXUIEXTENSION="C:\Program Files (x86)\WiX Toolset v3.11\bin\WixUIExtension.dll"

# package name will have git tag embedded in it. EXTRA_LABEL should be something 
# like "_win_x64", so msi file will be habit2-v2.2.5_win_x64.msi (if GIT_TAG is "v2.2.5")
PACKAGE_BASE=habit2_$(GIT_TAG)$(EXTRA_LABEL)
PACKAGE=$(BUILDDIR)\$(PACKAGE_BASE).msi
OBJS=$(OBJDIR)\HabitComponents.wixobj $(OBJDIR)\StimComponents.wixobj $(OBJDIR)\Habit2.wixobj
GENS=$(GENDIR)\HabitComponents.wxs $(GENDIR)\StimComponents.wxs

.PHONY: Package SignExe PatchSrc BuildSrc StagingPrep BuildSrc QMakeSrc DownloadSrc Directories
all: DIRECTORIES Package

Directories: $(BUILDDIR)\stamp-Directories

$(BUILDDIR)\stamp-Directories:
	if not exist "$(BUILDDIR)" (md $(BUILDDIR))
	if not exist "$(STAGINGDIR)" (md $(STAGINGDIR))
	if not exist "$(GENDIR)" (md $(GENDIR))
	if not exist "$(OBJDIR)" (md $(OBJDIR))
	if not exist "$(SRCDIR)" (md $(SRCDIR))
	copy /y /b NUL $@	
	
clean:
	@echo off
	if exist "$(BUILDDIR)"  (rd /S /Q $(BUILDDIR))
	if exist "$(TARGET)" (del $(TARGET))

# Light.exe returns error code 0xcc (unknown origin, no docs), but msi file still created. 
# The leading "-" tells nmake to ignore.

$(PACKAGE): $(BUILDDIR)\stamp-Directories $(BUILDDIR)\stamp-SignExe $(OBJS) 
	echo "Building $(PACKAGE)"
	-light -out $(PACKAGE) -ext $(WIXUIEXTENSION) -nologo $(OBJS)
	signtool sign /a /t http://timestamp.comodoca.com /f $(PFXFILE) /p "$(PFXKEY)" $(PACKAGE)

Package: Directories $(PACKAGE)


# Before making the target "SignExe", set filename and password of key above. 
SignExe: $(BUILDDIR)\stamp-SignExe

$(BUILDDIR)\stamp-SignExe: $(BUILDDIR)\stamp-StagingPrep
	signtool sign /a /t http://timestamp.comodoca.com /f $(PFXFILE) /p "$(PFXKEY)" $(STAGINGDIR)\habit2.exe
	copy /y /b NUL $@	

StagingPrep: $(BUILDDIR)\stamp-StagingPrep

$(BUILDDIR)\stamp-StagingPrep: $(BUILDDIR)\stamp-BuildSrc
	if not exist "$(STAGINGDIR)\gstreamer-plugins" (md $(STAGINGDIR)\gstreamer-plugins)
	if not exist "$(STAGINGDIR)\gstreamer-1.0" (md $(STAGINGDIR)\gstreamer-1.0)
	copy $(SRCDIR)\apps\habit\release\habit2.exe $(STAGINGDIR)
	copy $(SRCDIR)\libs\gstqt\release\gstqt.dll $(STAGINGDIR)\gstreamer-plugins
	windeployqt $(STAGINGDIR)\habit2.exe
	copy /y /b NUL $@

BuildSrc: $(BUILDDIR)\stamp-BuildSrc

$(BUILDDIR)\stamp-BuildSrc: $(BUILDDIR)\stamp-PatchSrc
	cd $(SRCDIR) && nmake -f Makefile release
	copy /y /b NUL $@	

PatchSrc: $(BUILDDIR)\stamp-PatchSrc

$(BUILDDIR)\stamp-PatchSrc: $(BUILDDIR)\stamp-QMakeSrc
	sed -i '/ProductVersion/s/\".*\"/\"$(DIST_VERSION)\"/' HabitConfiguration.wxi
	sed -i '/HABIT_VERSION/s/\".*\"/\"$(DIST_VERSION)\"/' $(SRCDIR)\apps\habit\version.h
	copy /y /b NUL $@

QMakeSrc: $(BUILDDIR)\stamp-QMakeSrc

$(BUILDDIR)\stamp-QMakeSrc: $(BUILDDIR)\stamp-DownloadSrc
	cd $(SRCDIR) && $(QMAKE) -o Makefile -recursive habit2.pro
	copy /y /b NUL $@	
	
DownloadSrc: DIRECTORIES $(BUILDDIR)\stamp-DownloadSrc

$(BUILDDIR)\stamp-DownloadSrc: DIRECTORIES
	curl -L -o $(BUILDDIR)\$(GIT_TAG).tar.gz https://github.com/djsperka/habit2-src/tarball/$(GIT_TAG)
	tar -C $(SRCDIR) --strip-components=1 -xzf $(BUILDDIR)\$(GIT_TAG).tar.gz
	copy /y /b NUL $@	
	
$(OBJDIR)\Habit2.wixobj: Habit2.wxs
	candle -dStimFolder=$(DISTSTIMDIR) -dStagingFolder=$(STAGINGDIR) -nologo -out $@ -arch x86 -ext "C:\Program Files (x86)\WiX Toolset v3.11\bin\\WixUIExtension.dll" $? 

$(OBJDIR)\StimComponents.wixobj: $(GENDIR)\StimComponents.wxs
	candle -dStimFolder=$(DISTSTIMDIR) -dStagingFolder=$(STAGINGDIR) -nologo -out $@ -arch x86 -ext "C:\Program Files (x86)\WiX Toolset v3.11\bin\\WixUIExtension.dll" $? 

$(OBJDIR)\HabitComponents.wixobj: $(GENDIR)\HabitComponents.wxs
	candle -dStimFolder=$(DISTSTIMDIR) -dStagingFolder=$(STAGINGDIR) -nologo -out $@ -arch x86 -ext "C:\Program Files (x86)\WiX Toolset v3.11\bin\\WixUIExtension.dll" $? 

$(GENDIR)\StimComponents.wxs:
	heat dir $(DISTSTIMDIR) -cg StimComponents -dr STIMDIR -sreg -srd -var var.StimFolder -nologo -ag -out $@
	
$(GENDIR)\HabitComponents.wxs:
	heat dir $(STAGINGDIR) -cg HabitComponents -dr INSTALLDIR -sreg -srd -var var.StagingFolder -nologo -ag -out $@
