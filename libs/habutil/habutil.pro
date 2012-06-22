TEMPLATE = lib
CONFIG += qt staticlib debug_and_release
QMAKE_CXXFLAGS += -fvisibility=hidden
DESTDIR = ../../lib
HEADERS +=	HLook.h \
			HLooker.h \
			HEvents.h \
			HAGState.h \
			HLook.h \
			HLookTransition.h \
			HLooker.h \
			HReadyTransition.h \
			HStartTrialState.h \
			HState.h \
			HStateMachine.h \
			HStimRequestState.h
SOURCES +=	HLook.cpp \
			HLooker.cpp \
			HAGState.cpp \
			HLookTransition.cpp \
			HReadyTransition.cpp \
			HStartTrialState.cpp \
			HState.cpp \
			HStateMachine.cpp \
			HStimRequestState.cpp
