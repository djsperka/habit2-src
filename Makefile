.DEFAULT_GOAL := all

%: Makefile.qmake
	@$(MAKE) -f Makefile.qmake $@

distclean: Makefile.qmake
	@$(MAKE) -f Makefile.qmake $@ 
	@find . -name "*~" -exec rm -v {} \;
	
#Makefile.qmake: habit2.pro
#	/usr/local/Trolltech/Qt-4.8.1-x86_64-no-dbus-dynamic/bin/qmake -o Makefile.qmake habit2.pro