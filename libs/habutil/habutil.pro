TEMPLATE = lib
CONFIG += qt staticlib debug_and_release
QMAKE_CXXFLAGS += -fvisibility=hidden
DESTDIR = ../../lib
HEADERS +=	HLook.h \
			HLooker.h \
			HLookDetector.h \
			HKeypadLookDetector.h \
			HEvents.h \
			HState.h \
			HStateMachine.h \
			HTrial.h \
			HElapsedTimer.h \
			HabitPlayer.h \
			HabitAudioPlayer.h \
			HabitVideoImagePlayer.h \
			HabitNonPlayer.h \
			StimulusSource.h \
			ImageWidget.h \
			HabitMediaManager.h
SOURCES +=	HLook.cpp \
			HLooker.cpp \
			HLookDetector.cpp \
			HKeypadLookDetector.cpp \
			HState.cpp \
			HStateMachine.cpp \
			HTrial.cpp \
			HElapsedTimer.cpp \
			HabitPlayer.cpp \
			HabitAudioPlayer.cpp \
			HabitVideoImagePlayer.cpp \
			HabitNonPlayer.cpp \
			StimulusSource.cpp \
			ImageWidget.cpp \
			HabitMediaManager.cpp
