.DEFAULT_GOAL := all

%:
	@$(MAKE) -f Makefile.qmake $@

distclean:
	@$(MAKE) -f Makefile.qmake $@ 
	@find . -name "*~" -exec rm -v {} \;