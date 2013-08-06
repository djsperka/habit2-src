TEMPLATE = lib
CONFIG += qt staticlib debug_and_release using_qt5

# Check if we are using qt version 5. If so, use Qt Multimedia
# variants of HAudioPlayer and HVideoImagePlayer

using_qt5 {
	QT += widgets multimedia multimediawidgets printsupport
	DEFINES += USING_QT5
	MM_HEADERS = H5VideoImagePlayer.h H5AudioPlayer.h H5MediaSoundPreviewPlayer.h
	MM_SOURCES = H5VideoImagePlayer.cpp H5AudioPlayer.cpp H5MediaSoundPreviewPlayer.cpp
} else {
	QT += phonon
	MM_HEADERS = HVideoImagePlayer.h HAudioPlayer.h habit/mediasoundpreviewplayer.h habit/imagewidget_original.h	
	MM_SOURCES = HVideoImagePlayer.cpp HAudioPlayer.cpp habit/mediasoundpreviewplayer.cpp habit/imagewidget_original.cpp
}

QMAKE_CXXFLAGS += -fvisibility=hidden
INCLUDEPATH += . ./habit
CONFIG(debug, debug|release) {
	DESTDIR = debug
} else {
	DESTDIR = release
}

HEADERS +=	HLook.h \
			HLooker.h \
			HLookDetector.h \
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
			HControlPanel.h \
			HMainWindow.h \
			HMediaManagerUtil.h \
			HTrialGenerator.h \
			HPhaseCriteria.h \
			HPhaseCriteriaUtil.h \
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
			HReliability.h \
			$$MM_HEADERS \
			HNonPlayer.h \
			habit/attentiongettersettings.h \
			habit/attentionsetupform.h \
			habit/cloneexperimentdialog.h \
			habit/configuration.h \
			habit/connection.h \
			habit/controlbaroptions.h \
			habit/controlbaroptionsform.h \
			habit/criterionsettings.h \
			habit/designsettings.h \
			habit/designsetupform.h \
#			habit/experimentresultsform.h \
			habit/experimentsettings.h \
			habit/experimentsettingsform.h \
			habit/exportmultipleresults.h \
#			habit/habitapp.h \
#			habit/habitcontrolpanel.h \
			habit/habituationsettings.h \
			habit/habituationsetupform.h \
			habit/habituationstimuliform.h \
			habit/habituationstimulitable.h \
			habit/maindao.h \
#			habit/mediamanager.h \
#			habit/mediaplayer.h \
			habit/monitormanager.h \
			habit/monitorsettings.h \
			habit/monitorsettingsform.h \
#			habit/OutputGenerator.h \
			habit/reliabilitiesform.h \
			habit/reliabilitysettings.h \
			habit/resource.h \
			habit/resource1.h \
			habit/resultsvieweroptions.h \
#			habit/resultviewerform.h \
			habit/resultviewersettings.h \
			habit/runsettings.h \
			habit/runsettingsform.h \
			habit/sleeper.h \
			habit/stimulisettings.h \
			habit/stimulusdisplayform.h \
			habit/stimulusdisplayinfo.h \
			habit/stimulusinfo.h \
			habit/stimulusinfoform.h \
			habit/stimulusplayer.h \
			habit/stimulussettings.h \
			habit/stimulussettingseditordialog.h \
			habit/subjectinformation.h \
			habit/subjectinformationwidget.h \
			habit/subjectsettings.h \
			habit/treewizard.h \
			habit/trialsinfo.h \
			habit/viewexperimentresultform.h


SOURCES +=	HLook.cpp \
			HLooker.cpp \
			HLookDetector.cpp \
			HKeypadLookDetector.cpp \
			HEvents.cpp \
			HState.cpp \
			HStateMachine.cpp \
			HTrial.cpp \
			HPhase.cpp \
			HElapsedTimer.cpp \
			HPlayer.cpp \
			HStimulusSource.cpp \
			HImageWidget.cpp \
			HMediaManager.cpp \
			HControlPanel.cpp \
			HMainWindow.cpp \
			HMediaManagerUtil.cpp \
			HTrialGenerator.cpp \
#			HOutputGenerator.cpp \
			HPhaseCriteria.cpp \
			HPhaseCriteriaUtil.cpp \
			HTrialLog.cpp \
			HPhaseLog.cpp \
			HEventLog.cpp \
			HPhaseChildState.cpp \
			HTrialChildState.cpp \
			HTypes.cpp \
			HResultsUtil.cpp \
			HResults.cpp \
			HResultsWidget.cpp \
			HResultsDialog.cpp \
			HApplication.cpp \
			HReliability.cpp \
			$$MM_SOURCES \
			HNonPlayer.cpp \
			habit/attentiongettersettings.cpp \
			habit/attentionsetupform.cpp \
			habit/cloneexperimentdialog.cpp \
			habit/configuration.cpp \
			habit/connection.cpp \
			habit/controlbaroptions.cpp \
			habit/controlbaroptionsform.cpp \
			habit/criterionsettings.cpp \
			habit/designsettings.cpp \
			habit/designsetupform.cpp \
#			habit/experimentresultsform.cpp \
			habit/experimentsettings.cpp \
			habit/experimentsettingsform.cpp \
			habit/exportmultipleresults.cpp \
#			habit/habitapp.cpp \
#			habit/habitcontrolpanel.cpp \
			habit/habituationsettings.cpp \
			habit/habituationsetupform.cpp \
			habit/habituationstimuliform.cpp \
			habit/habituationstimulitable.cpp \
#			habit/imagewidget_original.cpp \
#			habit/main.cpp \
			habit/maindao.cpp \
#			habit/mediamanager.cpp \
#			habit/mediaplayer.cpp \
			habit/monitormanager.cpp \
			habit/monitorsettings.cpp \
			habit/monitorsettingsform.cpp \
#			habit/OutputGenerator.cpp \
			habit/reliabilitiesform.cpp \
			habit/reliabilitysettings.cpp \
			habit/resultsvieweroptions.cpp \
#			habit/resultviewerform.cpp \
			habit/resultviewersettings.cpp \
			habit/runsettings.cpp \
			habit/runsettingsform.cpp \
			habit/stimulisettings.cpp \
			habit/stimulusdisplayform.cpp \
			habit/stimulusdisplayinfo.cpp \
			habit/stimulusinfo.cpp \
			habit/stimulusinfoform.cpp \
			habit/stimulusplayer.cpp \
			habit/stimulussettings.cpp \
			habit/stimulussettingseditordialog.cpp \
			habit/subjectinformation.cpp \
			habit/subjectinformationwidget.cpp \
			habit/subjectsettings.cpp \
			habit/treewizard.cpp \
			habit/trialsinfo.cpp \
			habit/viewexperimentresultform.cpp

RESOURCES = habit/resources.qrc

FORMS =		habit/cloneexperimentdialog.ui \
			habit/controlbaroptionsform.ui \
			habit/habitapp.ui \
			habit/monitorsettingsform.ui \
			habit/reliabilitiesform.ui \
			habit/resultviewerform.ui \
			habit/stimulusplayer.ui \
			habit/subjectinformationwidget.ui \
			habit/viewexperimentresultform.ui
