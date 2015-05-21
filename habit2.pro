TEMPLATE = subdirs
CONFIG += qt debug_and_release
SUBDIRS = habutil habit testmm
habutil.subdir = libs/habutil
habit.subdir = apps/habit
habit.depends = habutil
testmm.subdir = apps/testmm
testmm.depends = habutil

