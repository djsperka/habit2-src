TEMPLATE = lib
CONFIG += qt staticlib debug_and_release
QMAKE_CXXFLAGS += -fvisibility=hidden
INCLUDEPATH += . ./habit
CONFIG(debug, debug|release) {
	DESTDIR = debug
} else {
	DESTDIR = release
}


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
			HAudioPlayer.h \
			HNonPlayer.h \
			HVIPlayer.h \
			HStimulusSource.h \
			HImageWidget.h \
			HMediaManager.h \
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
			habit/HDBException.h \
			habit/HControlPanel.h \
			habit/attentiongettersettings.h \
			habit/attentionsetupform.h \
			habit/controlbaroptions.h \
			habit/controlbaroptionsform.h \
			habit/criterionsettings.h \
			habit/experimentsettings.h \
			habit/habituationsettings.h \
			habit/maindao.h \
			habit/monitorsettings.h \
			habit/runsettings.h \
			habit/stimulisettings.h \
			habit/stimulusdisplayinfo.h \
			habit/stimulusinfo.h \
			habit/stimulussettings.h \
			habit/subjectinformation.h \
			habit/subjectsettings.h \
			habit/HLookSettings.h \
			habit/HLookSettingsWidget.h \
			habit/HPhaseSettings.h \
			habit/HPhaseSettingsWidget.h \
			habit/HOrderSettings.h \
			habit/HMonitorSettingsWidget.h \
			habit/HStimulusDisplayInfoWidget.h \
			habit/HStimulusInfoWidget.h \
			habit/HStimulusSettingsWidget.h \
			habit/HStimuliSettingsWidget.h \
			habit/HStimulusSettingsListModel.h \
			habit/HStimulusSettingsListWidget.h \
			habit/HStimulusSettingsEditor.h \
			habit/HStimulusPreviewWidget.h \
			habit/H2MainWindow.h \
			habit/HExperimentListWidget.h \
			habit/HExperimentMain.h \
			habit/HExperimentTreeWidgetItem.h \ 
			habit/HExperimentListModel.h \
			habit/HExperimentListDelegate.h \
			habit/HExperimentListValidator.h \
			habit/HGlobalPreferencesDialog.h \
			habit/HStimulusOrder.h \
			habit/HStimulusOrderListModel.h \
			habit/HStimulusOrderListWidget.h \
			habit/HStimulusSettingsOrderImportUtil.h \
			habit/HStimulusOrderSelectionWidget.h \
			habit/HRunSettingsDialog.h \
			habit/HSubjectSettingsWidget.h \
			habit/HStimulusOrderEditor.h \
			habit/HStimulusSettingsList.h \
			habit/HStimulusNameListModel.h \
			habit/HStimulusOrderList.h \
			habit/HResultsExplorerDialog.h \
			habit/HMediaStatusWidget.h \
			habit/HExperimentStatusWidget.h \
			habit/HHabituationSetupWidget.h \
			habit/HStimulusOrderListItemDelegate.h \
			habit/HStimulusNameLabelTableModel.h
		

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
			HAudioPlayer.cpp \
			HNonPlayer.cpp \
			HVIPlayer.cpp \
			HStimulusSource.cpp \
			HImageWidget.cpp \
			HMediaManager.cpp \
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
			habit/HDBException.cpp \
			habit/HControlPanel.cpp \
			habit/attentiongettersettings.cpp \
			habit/attentionsetupform.cpp \
			habit/controlbaroptions.cpp \
			habit/controlbaroptionsform.cpp \
			habit/criterionsettings.cpp \
			habit/experimentsettings.cpp \
			habit/habituationsettings.cpp \
			habit/maindao.cpp \
			habit/monitorsettings.cpp \
			habit/runsettings.cpp \
			habit/stimulisettings.cpp \
			habit/stimulusdisplayinfo.cpp \
			habit/stimulusinfo.cpp \
			habit/stimulussettings.cpp \
			habit/subjectinformation.cpp \
			habit/subjectsettings.cpp \
			habit/HLookSettings.cpp \
			habit/HLookSettingsWidget.cpp \
			habit/HPhaseSettings.cpp \
			habit/HPhaseSettingsWidget.cpp \
			habit/HOrderSettings.cpp \
			habit/HMonitorSettingsWidget.cpp \
			habit/HStimulusDisplayInfoWidget.cpp \
			habit/HStimulusInfoWidget.cpp \
			habit/HStimulusSettingsWidget.cpp \
			habit/HStimuliSettingsWidget.cpp \
			habit/HStimulusSettingsListModel.cpp \
			habit/HStimulusSettingsListWidget.cpp \
			habit/HStimulusSettingsEditor.cpp \
			habit/HStimulusPreviewWidget.cpp \
			habit/H2MainWindow.cpp \
			habit/HExperimentListWidget.cpp \
			habit/HExperimentMain.cpp \
			habit/HExperimentListModel.cpp \
			habit/HExperimentListDelegate.cpp \
			habit/HExperimentListValidator.cpp \
			habit/HGlobalPreferencesDialog.cpp \
			habit/HStimulusOrder.cpp \
			habit/HStimulusOrderListModel.cpp \
			habit/HStimulusOrderListWidget.cpp \
			habit/HStimulusSettingsOrderImportUtil.cpp \
			habit/HStimulusOrderSelectionWidget.cpp \
			habit/HRunSettingsDialog.cpp \
			habit/HSubjectSettingsWidget.cpp \
			habit/HStimulusOrderEditor.cpp \
			habit/HStimulusSettingsList.cpp \
			habit/HStimulusNameListModel.cpp \
			habit/HStimulusOrderList.cpp \
			habit/HResultsExplorerDialog.cpp \
			habit/HMediaStatusWidget.cpp \
			habit/HExperimentStatusWidget.cpp \
			habit/HHabituationSetupWidget.cpp \
			habit/HStimulusOrderListItemDelegate.cpp \
			habit/HStimulusNameLabelTableModel.cpp
			

RESOURCES = habit/resources.qrc

FORMS =		\
#habit/cloneexperimentdialog.ui \
			habit/controlbaroptionsform.ui \
			habit/monitorsettingsform.ui \
			habit/reliabilitiesform.ui \
			habit/resultviewerform.ui \
			habit/stimulusplayer.ui \
			habit/subjectinformationwidget.ui \
			habit/viewexperimentresultform.ui \
			habit/HMonitorSettingsForm.ui \
			habit/HStimulusDisplayInfoForm.ui \
			habit/HStimulusInfoForm.ui \
			habit/HStimulusOrderSelectionForm.ui \
			habit/HSubjectSettingsForm.ui \
			habit/HStimulusOrderEditorform.ui \
			habit/HResultsExplorerForm.ui \
			habit/HMediaStatusForm.ui \
			habit/HExperimentStatusForm.ui \
			habit/HLookSettingsForm.ui \
			habit/HPhaseSettingsForm.ui \
			habit/HHabituationSetupForm.ui 
			