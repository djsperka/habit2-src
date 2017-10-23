MAKEFILE = Makefile.qmake
TEMPLATE = subdirs
CONFIG += qt debug_and_release
SUBDIRS = habutil habit hg2 testmm hvw

# habresults gplayer testmm hgstplayer 
habutil.subdir = libs/habutil
habit.subdir = apps/habit
habit.depends = habutil
habresults.subdir = apps/habresults
habresults.depends = habutil
testmm.subdir = apps/testmm
testmm.depends = habutil
testkeypadld.subdir = apps/testkeypadld
testkeypadld.depends = habutil
q5player.subdir = apps/q5player
q5player.depends = habutil
vlcplayer.subdir = apps/vlcplayer
vlcplayer.depends = habutil
gplayer.subdir = apps/gplayer
gplayer.depends = habutil
hgstplayer.file = apps/gstx/hgstplayer.pro
hgstplayer.depends = habutil
hg2.subdir = apps/hg2
hg2.depends = habutil
hvw.subdir = apps/hvw
hvw.depends = habutil
