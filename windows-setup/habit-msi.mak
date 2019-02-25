# Note: $(MAKEDIR) is the location of this makefile. It is assumed that the 
# habit2 root src folder is the parent folder to the folde where this makefile
# resides.
BUILDDIR=$(MAKEDIR)\build
STAGINGDIR=$(BUILDDIR)\staging
OBJDIR=$(BUILDDIR)\obj
GENDIR=$(BUILDDIR)\generated

# these are for stuff within the src tree
SRCROOT=$(MAKEDIR)\..
DISTSTIMDIR=$(SRCROOT)\distribution\stim
DISTEXEDIR=$(SRCROOT)\apps\habit\release

# wix plugin we use
WIXUIEXTENSION="C:\Program Files (x86)\WiX Toolset v3.11\bin\WixUIExtension.dll"

# this should come in as a def'd parameter, or part of it should be...?
TARGET = habit.msi
OBJS = $(OBJDIR)\HabitComponents.wixobj $(OBJDIR)\StimComponents.wixobj $(OBJDIR)\Habit2.wixobj
GENS = $(GENDIR)\HabitComponents.wxs $(GENDIR)\StimComponents.wxs

all: DIRECTORIES $(TARGET)

DIRECTORIES:
	if not exist "$(BUILDDIR)" (md $(BUILDDIR))
	if not exist "$(STAGINGDIR)" (md $(STAGINGDIR))
	if not exist "$(GENDIR)" (md $(GENDIR))
	if not exist "$(OBJDIR)" (md $(OBJDIR))
	
clean:
	@echo off
	if exist "$(BUILDDIR)"  (rd /S /Q $(BUILDDIR))
	if exist "$(TARGET)" (del $(TARGET))
	
$(TARGET): StagingPrep $(OBJS) 
	echo "Building TARGET $(TARGET)
	light -out $@ -ext $(WIXUIEXTENSION) -nologo $(OBJS)

StagingPrep:
	if not exist "$(STAGINGDIR)\gstreamer-plugins" (md $(STAGINGDIR)\gstreamer-plugins)
	if not exist "$(STAGINGDIR)\gstreamer-1.0" (md $(STAGINGDIR)\gstreamer-1.0)
	copy $(SRCROOT)\apps\habit\release\habit2.exe $(STAGINGDIR)
	copy $(SRCROOT)\libs\gstqt\release\gstqt.dll $(STAGINGDIR)\gstreamer-plugins
	windeployqt $(STAGINGDIR)\habit2.exe

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
