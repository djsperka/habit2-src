#
#
# to run:
#  distribution:	GIT_TAG=v2-2-5-rc3 \
#					QT_BIN=/Users/dan/Qt/5.9.1/clang_64/bin/ \
#					PKG_CONFIG_PATH=/Library/Frameworks/GStreamer.framework/Versions/Current/Libraries/pkgconfig/ \
#					GSTREAMER_LIBS_RELOCATED=/Users/dan/gstreamer-1.12.4-relocated.tar \
#					SIGUSERID=WWAFVH26T8
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
$(info "STIM_VERSION = " $(STIM_VERSION))
$(info "DISPLAY_TITLE = " $(DISPLAY_TITLE))
$(info "SIGUSERID = " $(SIGUSERID))


QMAKE=$(QT_BIN)/qmake
MACDEPLOYQT=$(QT_BIN)/macdeployqt
RELOC="./myreloc.sh"
$(info "RELOC = " $(RELOC))
CREATEDMG=$(shell which create-dmg)
$(info "CREATEDMG = " $(CREATEDMG))


## generate args for GENPKGPROJ because some conditionals are needed before running... might have min and max version, but 
## might only have min version. Should have at least one of them!
#GENPKGPROJ=$(CURDIR)/genpkgproj.py
#GENPKGPROJ_ARGS=-v -p $(UNSIGNED_PACKAGE_BASE) -d $(DIST_VERSION) -s $(STIM_VERSION) -t "$(DISPLAY_TITLE)"
#ifdef OSMIN
#GENPKGPROJ_ARGS+=-m $(OSMIN)
#endif
#ifdef OSMAX
#GENPKGPROJ_ARGS+=-x $(OSMAX)
#endif
BUILDDIR=$(CURDIR)/build
SRCDIR=$(BUILDDIR)/src
DISTDIR=$(BUILDDIR)/dist

UNSIGNED_PACKAGE_BASE=habit2-$(GIT_TAG)$(EXTRA_LABEL)-notsigned
SIGNED_PACKAGE_BASE=habit2-$(GIT_TAG)$(EXTRA_LABEL)

all: package

.PHONY: all directories clean getsrc qmake build reloc dmg

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


$(BUILDDIR)/stamp-reloc: $(BUILDDIR)/stamp-build
	$(RELOC) $(SRCDIR)/apps/habit/release habit2 ff.txt pkgs.txt $(DISTDIR)
	touch $@


reloc: $(BUILDDIR)/stamp-reloc
	
#
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# sign bundle with my handy-dandy signer

$(BUILDDIR)/stamp-sign-app: $(BUILDDIR)/stamp-reloc
	./habit2-sign.py $(DISTDIR)/habit2.app $(SIGUSERID)
	touch $@

sign-app: $(BUILDDIR)/stamp-sign-app

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Create input file for 'packages' then build package
#
#  GENPKGPROJ line replaced this
#	sed -e 's/DISPLAY-TITLE/Habit2 ($(GIT_TAG))/g' -e  's/PKG-FILE-BASENAME/$(UNSIGNED_PACKAGE_BASE)/g' -e 's/DIST-VERSION/$(GIT_TAG)/g' -e 's/STIM-VERSION/$(GIT_TAG)/g' macdist-template.pkgproj > build/macdist-$(GIT_TAG).pkgproj

$(BUILDDIR)/stamp-dmg: $(BUILDDIR)/stamp-sign-app
	$(CREATEDMG) 	--volicon $(SRCDIR)/apps/habit/habit2.icns \
					--volname "Habit2 Installer" \
					--window-pos 200 120 \
					--window-size 800 400 \
					--icon-size 100 \
					--icon "habit2.app" 200 190 \
					--hide-extension "habit2.app" \
					--app-drop-link 600 185 \
					"Habit2-Installer.dmg" \
					$(DISTDIR)
	codesign -s "Developer ID Application: University of California- Davis (WWAFVH26T8)" --timestamp Habit2-Installer.dmg
	touch $@

dmg: $(BUILDDIR)/stamp-dmg

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Sign package 
$(BUILDDIR)/stamp-sign-package: $(BUILDDIR)/stamp-package
	productsign --sign "Developer ID Installer: University of California- Davis (WWAFVH26T8)" build/$(UNSIGNED_PACKAGE_BASE).pkg build/$(SIGNED_PACKAGE_BASE).pkg
	touch $@
	
sign-package: $(BUILDDIR)/stamp-sign-package


#curl -L https://github.com/djsperka/habit2-src/tarball/v2-2-5-rc3 | tar zx
#0R
#git clone https://github.com/djsperka/habit2-src.git build

#PATH=~/Qt/5.9.1/clang_64/bin:$PATH qmake -o Makefile -recursive habit2.pro
#make release
#mkdir apps/habit/release/habit2.app/Contents/Frameworks
#tar xf ~/git/habit2-src/distribution/gstreamer-1.12.4-relocated.tar -C apps/habit/release/habit2.app/Contents/Frameworks/
#osxrelocator libs/gstqt/release/ /Library/Frameworks/GStreamer.framework @executable_path/../Frameworks/GStreamer.framework
#cp libs/gstqt/release/libgstqt.dylib apps/habit/release/habit2.app/Contents/Frameworks/GStreamer.framework/Versions/Current/lib/gstreamer-1.0/
#PATH=~/Qt/5.9.1/clang_64/bin/:$PATH macdeployqt apps/habit/release/habit2.app
