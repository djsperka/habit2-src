#MAKEFILE = Makefile.qmake
TEMPLATE = subdirs
CONFIG += qt debug_and_release
SUBDIRS = habutil habit gstqt testmm

# habresults gplayer testmm hgstplayer testmisc

habutil.subdir = libs/habutil
habit.subdir = apps/habit
habit.depends = habutil gstqt
hg3.subdir = apps/hg3
hg3.depends = habutil
gstqt.subdir = libs/gstqt
fdhunt.subdir = apps/fdhunt
testmm.subdir = apps/testmm
testmm.depends = habutil
testmisc.subdir = apps/testmisc
testmisc.depends = habutil
gstsp.subdir = apps/gstsp
gstsp.depends = habutil

