TEMPLATE = lib
CONFIG += qt staticlib debug_and_release
QMAKE_CXXFLAGS += -fvisibility=hidden
DESTDIR = ../../lib
HEADERS +=	HLook.h \
			HLooker.h \
			HEvents.h \
			HAGState.h \
			HReadyTransition.h \
			HStartTrialState.h \
			HState.h \
			HStateMachine.h \
			HStimRequestState.h \
			HTrial.h \
			HabitPlayer.h \
			HabitAudioPlayer.h \
			HabitVideoImagePlayer.h \
			StimulusSource.h \
			ImageWidget.h \
			HabitMediaManager.h
SOURCES +=	HLook.cpp \
			HLooker.cpp \
			HAGState.cpp \
			HReadyTransition.cpp \
			HStartTrialState.cpp \
			HState.cpp \
			HStateMachine.cpp \
			HStimRequestState.cpp \
			HTrial.cpp \
			HabitPlayer.cpp \
			HabitAudioPlayer.cpp \
			HabitVideoImagePlayer.cpp \
			StimulusSource.cpp \
			ImageWidget.cpp \
			HabitMediaManager.cpp
