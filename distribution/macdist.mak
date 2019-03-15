#
#
# to run:
#  distribution:	GIT_TAG=v2-2-5-rc3 QT_BIN=/Users/dan/Qt/5.9.1/clang_64/bin/ \
#					PKG_CONFIG_PATH=/Library/Frameworks/GStreamer.framework/Versions/Current/Libraries/pkgconfig/ \
#					GSTREAMER_LIBS_RELOCATED=/Users/dan/gstreamer-1.12.4-relocated.tar \
#					make -f macdist.mak macdeployqt

"GIT_TAG = " v2-2-5-rc3

# Inputs
#
# 1. tag/branch/commit - e.g. "v2-2-5-rc3"
#
# 2. qmake path - specify qt version when running qmake to generate makefiles. must also do so later when running macdeployqt
# 3. PKG_CONFIG_PATH - specify gstreamer version, effectively, by picking the appropriate location of .pc files
# 4. GSTREAMER_LIBS_RELOCATED - full path to tar file with relocated gstreamer framework. Should be folder "GStreamer.framework"
#    will be unpacked at dist/habit2.app/   
#    *** THE TAR'D FRAMEWORK MUST BE SAME VERSION AS THAT SPECIFIED WITH PKG_CONFIG_PATH
# Outputs
#
# always put distributable app in folder called "app"
# "app" folder is created/cleaned at start of build here

$(info "GIT_TAG = " $(GIT_TAG))
$(info "QT_BIN = " $(QT_BIN))
$(info "PKG_CONFIG_PATH = " $(PKG_CONFIG_PATH))

QMAKE=$(QT_BIN)/qmake
MACDEPLOYQT=$(QT_BIN)/macdeployqt

BUILDDIR=$(CURDIR)/build
SRCDIR=$(BUILDDIR)/src
DISTDIR=$(BUILDDIR)/dist

all: package

.PHONY: all directories clean getsrc qmake build copy-app copy-gstreamer copy-plugin relocate-app macdeployqt package

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# empty out build folder

clean:
	rm -rf $(BUILDDIR)/*


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# create folders for building

$(BUILDDIR)/stamp-directories:
	mkdir -p $(DISTDIR)
	mkdir -p $(SRCDIR)
	touch $@

directories: $(BUILDDIR)/stamp-directories

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# get source from github. GIT_TAG must be a tag or branch or SHA-thingy

$(BUILDDIR)/stamp-getsrc: directories
	cd $(SRCDIR) && (curl -L https://github.com/djsperka/habit2-src/tarball/$(GIT_TAG) | tar --strip=1 -zx)
	touch $@

getsrc: $(BUILDDIR)/stamp-getsrc

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# run qmake to generate makefiles

$(BUILDDIR)/stamp-qmake: getsrc
	cd $(SRCDIR) && $(QMAKE) -recursive -o Makefile habit2.pro
	touch $@

qmake: $(BUILDDIR)/stamp-qmake

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# compile in release mode

$(BUILDDIR)/stamp-build: qmake
	cd $(SRCDIR) && make release
	touch $@

build: $(BUILDDIR)/stamp-build

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# copy freshly-build application to dist folder

$(BUILDDIR)/stamp-copy-app: build
	tar -C $(SRCDIR)/apps/habit/release -cf - habit2.app | tar -C $(DISTDIR) -xf -
	touch $@

copy-app: $(BUILDDIR)/stamp-copy-app

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# relocate application to use gstreamer frameworks in its own tree
$(BUILDDIR)/stamp-relocate-app: copy-app
	osxrelocator $(BUILDDIR)/dist/habit2.app/Contents/MacOS /Library/Frameworks/GStreamer.framework @executable_path/../Frameworks/GStreamer.framework
	touch $@
	
relocate-app: $(BUILDDIR)/stamp-relocate-app

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# copy gstreamer libs to dist
$(BUILDDIR)/stamp-copy-gstreamer: relocate-app
	mkdir -p $(DISTDIR)/habit2.app/Contents/Frameworks
	tar -C $(DISTDIR)/habit2.app/Contents/Frameworks -xf $(GSTREAMER_LIBS_RELOCATED)
	touch $@
	
copy-gstreamer: $(BUILDDIR)/stamp-copy-gstreamer


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# relocate plugin, copy to dist
# needs gstreamer to be present so dirs are present
$(BUILDDIR)/stamp-copy-plugin: copy-gstreamer
	osxrelocator $(SRCDIR)/libs/gstqt/release /Library/Frameworks/GStreamer.framework @executable_path/../Frameworks/GStreamer.framework
	cp $(SRCDIR)/libs/gstqt/release/libgstqt.dylib $(DISTDIR)/habit2.app/Contents/Frameworks/GStreamer.framework/Versions/Current/Libraries/gstreamer-1.0
	touch $@

copy-plugin: $(BUILDDIR)/stamp-copy-plugin

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# run macdeployqt on relocated app
$(BUILDDIR)/stamp-macdeployqt: copy-plugin
	$(MACDEPLOYQT) $(DISTDIR)/habit2.app
	touch $@

macdeployqt: $(BUILDDIR)/stamp-macdeployqt

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Create input file for 'packages' then build package
$(BUILDDIR)/stamp-package: macdeployqt
	sed -e 's/DISPLAY-TITLE/Habit2 ($(GIT_TAG))/g' -e 's/PKG-FILE-BASENAME/habit2-$(GIT_TAG)/g' -e 's/DIST-VERSION/$(GIT_TAG)/g' -e 's/STIM-VERSION/$(GIT_TAG)/g' macdist-template.pkgproj > build/macdist-$(GIT_TAG).pkgproj
	packagesbuild --reference-folder $(CURDIR) build/macdist-$(GIT_TAG).pkgproj 
	touch $@

package: $(BUILDDIR)/stamp-package


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
