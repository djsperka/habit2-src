#MAKEFILE = Makefile.qmake
TEMPLATE = subdirs
CONFIG += qt debug_and_release
SUBDIRS = habutil habit testmm

# testmisc gstsp

#gstqt

# habresults gplayer testmm hgstplayer 
habutil.subdir = libs/habutil
habit.subdir = apps/habit
habit.depends = habutil
hg3.subdir = apps/hg3
hg3.depends = habutil
fdhunt.subdir = apps/fdhunt
testmm.subdir = apps/testmm
testmm.depends = habutil
testmisc.subdir = apps/testmisc
testmisc.depends = habutil
gstsp.subdir = apps/gstsp
gstsp.depends = habutil

