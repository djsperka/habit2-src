TEMPLATE = lib
CONFIG += qt staticlib debug_and_release
QMAKE_CXXFLAGS += -fvisibility=hidden
DESTDIR = ../../lib
HEADERS +=	HLook.h \
			HLooker.h \
			HEvents.h \
			HState.h \
			HStateMachine.h \
			HTrial.h \
			HabitPlayer.h \
			HabitAudioPlayer.h \
			HabitVideoImagePlayer.h \
			StimulusSource.h \
			ImageWidget.h \
			HabitMediaManager.h \
			HLookEventFilter.h
SOURCES +=	HLook.cpp \
			HLooker.cpp \
			HState.cpp \
			HStateMachine.cpp \
			HTrial.cpp \
			HabitPlayer.cpp \
			HabitAudioPlayer.cpp \
			HabitVideoImagePlayer.cpp \
			StimulusSource.cpp \
			ImageWidget.cpp \
			HabitMediaManager.cpp \
			HLookEventFilter.cpp
