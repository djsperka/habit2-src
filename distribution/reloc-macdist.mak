#
#
# to run:
#  distribution:	GIT_TAG=v2-2-5-rc3 \
#					QT_BIN=/Users/dan/Qt/5.9.1/clang_64/bin/ \
#					PKG_CONFIG_PATH=/Library/Frameworks/GStreamer.framework/Versions/Current/Libraries/pkgconfig/ \
#					SIGID=WWAFVH26T8
#					make -f macdist.mak macdeployqt
# Inputs
#
# 1. tag/branch/commit - e.g. "v2-2-5-rc3"
#
# 2. qmake path - specify qt version when running qmake to generate makefiles. must also do so later when running macdeployqt
# 3. PKG_CONFIG_PATH - specify gstreamer version, effectively, by picking the appropriate location of .pc files
# 4. reloc.sh must be on the path
#    *** THE TAR'D FRAMEWORK MUST BE SAME VERSION AS THAT SPECIFIED WITH PKG_CONFIG_PATH
# Outputs
#
# always put distributable app in folder called "app"
# "app" folder is created/cleaned at start of build here

$(info "PATH = " $(PATH))
$(info "GIT_TAG = " $(GIT_TAG))
$(info "QT_BIN = " $(QT_BIN))
$(info "PKG_CONFIG_PATH = " $(PKG_CONFIG_PATH))
$(info "EXTRA_LABEL = $(EXTRA_LABEL)")
$(info "DIST_VERSION = " $(DIST_VERSION))
#$(info "STIM_VERSION = " $(STIM_VERSION))
#$(info "DISPLAY_TITLE = " $(DISPLAY_TITLE))
$(info "SIGID = " $(SIGID))


QMAKE=$(QT_BIN)/qmake
MACDEPLOYQT=$(QT_BIN)/macdeployqt
RELOC="./myreloc.sh"
$(info "RELOC = " $(RELOC))
CREATEDMG=$(shell which create-dmg)
$(info "CREATEDMG = " $(CREATEDMG))


BUILDDIR=$(CURDIR)/build
SRCDIR=$(BUILDDIR)/src
DISTDIR=$(BUILDDIR)/dist

UNSIGNED_PACKAGE_BASE=habit2-$(GIT_TAG)$(EXTRA_LABEL)-notsigned
SIGNED_PACKAGE_BASE=habit2-$(GIT_TAG)$(EXTRA_LABEL)

all: package

.PHONY: all directories clean getsrc qmake build reloc deploy dmg

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# empty out build folder

clean:
	rm -rf $(BUILDDIR)/*


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# create folders for building

$(BUILDDIR)/stamp-directories:
	#mkdir -p $(DISTDIR)
	mkdir -p $(SRCDIR)
	touch $@

directories: $(BUILDDIR)/stamp-directories

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# get source from github. GIT_TAG must be a tag or branch or SHA-thingy

$(BUILDDIR)/stamp-getsrc: $(BUILDDIR)/stamp-directories
	cd $(SRCDIR) && (curl -L https://api.github.com/repos/djsperka/habit2-src/tarball/$(GIT_TAG) | tar --strip=1 -x)
	touch $@

getsrc: $(BUILDDIR)/stamp-getsrc

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# run qmake to generate makefiles

$(BUILDDIR)/stamp-qmake: $(BUILDDIR)/stamp-getsrc
	cd $(SRCDIR) && $(QMAKE) -recursive -o Makefile habit2.pro
	touch $@

qmake: $(BUILDDIR)/stamp-qmake


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# modify version number in main habit dialog

$(BUILDDIR)/stamp-patch-src: $(BUILDDIR)/stamp-qmake
	sed -i -e '/HABIT_VERSION/s/\".*\"/\"$(DIST_VERSION)\"/' $(SRCDIR)/apps/habit/version.h
	touch $@

patch-src: $(BUILDDIR)/stamp-patch-src

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# compile in release mode

$(BUILDDIR)/stamp-build: $(BUILDDIR)/stamp-patch-src
	cd $(SRCDIR) && make sub-apps-habit
	touch $@

build: $(BUILDDIR)/stamp-build

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# relocate libs (not qt)

$(BUILDDIR)/stamp-reloc: $(BUILDDIR)/stamp-build
	$(RELOC) $(SRCDIR)/apps/habit/release habit2 habit-plugin.txt pkgs.txt $(DISTDIR)
	touch $@

reloc: $(BUILDDIR)/stamp-reloc

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# do Qt relocation stuff, pull Qt libs into bundle

$(BUILDDIR)/stamp-deploy: $(BUILDDIR)/stamp-reloc
	$(MACDEPLOYQT) $(DISTDIR)/habit2.app
	touch $@
	
deploy: $(BUILDDIR)/stamp-deploy

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# sign bundle with my handy-dandy signer
#	./habit2-sign.py $(DISTDIR)/habit2.app $(SIGUSERID)
# Now signing with new handy-dandy signer

$(BUILDDIR)/stamp-sign-app: $(BUILDDIR)/stamp-deploy
	./sign.sh $(DISTDIR) $(SIGID)
	touch $@

sign-app: $(BUILDDIR)/stamp-sign-app

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Create dmg file and sign it.
#

$(BUILDDIR)/stamp-dmg: $(BUILDDIR)/stamp-sign-app
	$(CREATEDMG) 	--volicon $(SRCDIR)/apps/habit/habit2.icns \
					--volname "Habit2 Installer" \
					--window-pos 200 120 \
					--window-size 800 400 \
					--icon-size 100 \
					--icon "habit2.app" 200 190 \
					--hide-extension "habit2.app" \
					--app-drop-link 600 185 \
					"$(SIGNED_PACKAGE_BASE).dmg" \
					$(DISTDIR)/signed
	codesign -s $(SIGID) --timestamp $(SIGNED_PACKAGE_BASE).dmg
	touch $@

dmg: $(BUILDDIR)/stamp-dmg


