MAKEFILE = Makefile.qmake
TEMPLATE = subdirs
CONFIG += qt debug_and_release
SUBDIRS = habutil habit habresults
habutil.subdir = libs/habutil
habit.subdir = apps/habit
habit.depends = habutil
habresults.subdir = apps/habresults
habresults.depends = habutil
testmm.subdir = apps/testmm
testmm.depends = habutil
testkeypadld.subdir = apps/testkeypadld
testkeypadld.depends = habutil
