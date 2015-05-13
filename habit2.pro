TEMPLATE = subdirs
CONFIG += qt debug_and_release
SUBDIRS = habutil habit
habutil.subdir = libs/habutil
habit.subdir = apps/habit
habit.depends = habutil

