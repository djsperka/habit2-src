#############################################################################
# Makefile for building: ../../bin/habit.app/Contents/MacOS/habit
# Generated by qmake (2.01a) (Qt 4.8.1) on: Wed Oct 3 15:22:33 2012
# Project:  habit.pro
# Template: app
# Command: /usr/local/Trolltech/Qt-4.8.1-x86_64-no-dbus-dynamic/bin/qmake -spec /usr/local/Trolltech/Qt-4.8.1-x86_64-no-dbus-dynamic/mkspecs/macx-xcode -o habit.xcodeproj/project.pbxproj habit.pro
#############################################################################

MOC       = /usr/local/Trolltech/Qt-4.8.1-x86_64-no-dbus-dynamic/bin/moc
UIC       = /usr/local/Trolltech/Qt-4.8.1-x86_64-no-dbus-dynamic/bin/uic
LEX       = flex
LEXFLAGS  = 
YACC      = yacc
YACCFLAGS = -d
DEFINES       = -DQT_NO_DEBUG -DQT_PHONON_LIB -DQT_SQL_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
INCPATH       = -I/usr/local/Trolltech/Qt-4.8.1-x86_64-no-dbus-dynamic/mkspecs/macx-xcode -I. -I/usr/local/Trolltech/Qt-4.8.1-x86_64-no-dbus-dynamic/lib/QtCore.framework/Versions/4/Headers -I/usr/local/Trolltech/Qt-4.8.1-x86_64-no-dbus-dynamic/include/QtCore -I/usr/local/Trolltech/Qt-4.8.1-x86_64-no-dbus-dynamic/lib/QtGui.framework/Versions/4/Headers -I/usr/local/Trolltech/Qt-4.8.1-x86_64-no-dbus-dynamic/include/QtGui -I/usr/local/Trolltech/Qt-4.8.1-x86_64-no-dbus-dynamic/lib/QtSql.framework/Versions/4/Headers -I/usr/local/Trolltech/Qt-4.8.1-x86_64-no-dbus-dynamic/include/QtSql -I/usr/local/Trolltech/Qt-4.8.1-x86_64-no-dbus-dynamic/lib/phonon.framework/Versions/4/Headers -I/usr/local/Trolltech/Qt-4.8.1-x86_64-no-dbus-dynamic/include/phonon -I/usr/local/Trolltech/Qt-4.8.1-x86_64-no-dbus-dynamic/include -I../../libs/habutil -I../../libs/habutil/habit -I/usr/local/Trolltech/Qt-4.8.1-x86_64-no-dbus-dynamic/include/phonon_compat -I. -I/usr/local/include -I/System/Library/Frameworks/CarbonCore.framework/Headers -F/usr/local/Trolltech/Qt-4.8.1-x86_64-no-dbus-dynamic/lib
DEL_FILE  = rm -f
MOVE      = mv -f

IMAGES = 
PARSERS =
preprocess: $(PARSERS) compilers
clean preprocess_clean: parser_clean compiler_clean

parser_clean:
check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compilers:
compiler_objective_c_make_all:
compiler_objective_c_clean:
compiler_moc_header_make_all:
compiler_moc_header_clean:
compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_rez_source_make_all:
compiler_rez_source_clean:
compiler_uic_make_all:
compiler_uic_clean:
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: 

