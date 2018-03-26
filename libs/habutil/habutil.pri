
RESOURCES = habit/resources.qrc
message("inside habutil.pri")
message("reading habit-files.pri")
include(habit-files.pri)
message("reading hgst-files.pri")
include(hgst-files.pri)
message("reading wizards-files.pri")
include(wizards-files.pri)

HEADERS +=	HLook.h \
			HLooker.h \
			HLookerReprocessor.h \
			HTrialKey.h \
			HTrialResult.h \
			HTrialScanner.h \
			HLookDetector.h \
			HLookDetectorUtil.h \
			HKeypadLookDetector.h \
			HEvents.h \
			HQEvents.h \
			HState.h \
			HLogState.h \
			HStateMachine.h \
			HTrial.h \
			HPhase.h \
			HElapsedTimer.h \
			HPlayer.h \
			HNonPlayer.h \
			HStimulusSource.h \
			HImageWidget.h \
			HMediaManager.h \
			HPreviewMediaManager.h \
			HMediaManagerUtil.h \
			HTrialGenerator.h \
			HPhaseCriteria.h \
			HPhaseCriteriaUtil.h \
			HDBUtil.h \
			HWorkspaceUtil.h \
			HTrialLog.h \
			HPhaseLog.h \
			HEventLog.h \
			HExperiment.h \
			HExperimentChildState.h \
			HPhaseChildState.h \
			HTrialChildState.h \
			HTypes.h \
			HResultsUtil.h \
			HResults.h \
			HResultsWidget.h \
			HResultsDialog.h \
			HApplication.h \
			HStimulusUtil.h \
			HExperimentUtil.h \
			HTestingInputWrangler.h \
			HNameValidator.h
		

SOURCES +=	HLook.cpp \
			HLooker.cpp \
			HLookerReprocessor.cpp \
			HTrialKey.cpp \
			HTrialResult.cpp \
			HTrialScanner.cpp \
			HLookDetector.cpp \
			HLookDetectorUtil.cpp \
			HKeypadLookDetector.cpp \
			HEvents.cpp \
			HState.cpp \
			HStateMachine.cpp \
			HTrial.cpp \
			HPhase.cpp \
			HElapsedTimer.cpp \
			HPlayer.cpp \
			HNonPlayer.cpp \
			HStimulusSource.cpp \
			HImageWidget.cpp \
			HMediaManager.cpp \
			HPreviewMediaManager.cpp \
			HMediaManagerUtil.cpp \
			HTrialGenerator.cpp \
			HPhaseCriteria.cpp \
			HPhaseCriteriaUtil.cpp \
			HDBUtil.cpp \
			HWorkspaceUtil.cpp \
			HTrialLog.cpp \
			HPhaseLog.cpp \
			HEventLog.cpp \
			HExperiment.cpp \
			HPhaseChildState.cpp \
			HTrialChildState.cpp \
			HTypes.cpp \
			HResultsUtil.cpp \
			HResults.cpp \
			HResultsWidget.cpp \
			HResultsDialog.cpp \
			HApplication.cpp \
			HStimulusUtil.cpp \
			HExperimentUtil.cpp \
			HTestingInputWrangler.cpp \
			HNameValidator.cpp
			
			

