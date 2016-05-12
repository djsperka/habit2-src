.DEFAULT_GOAL := all

%: Makefile.qmake
	@$(MAKE) -f Makefile.qmake $@

distclean: Makefile.qmake
	@$(MAKE) -f Makefile.qmake $@ 
	@find . -name "*~" -exec rm -v {} \;
	
Makefile.qmake: habit2.pro
	qmake -o Makefile.qmake habit2.pro