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
			HLookDetector.h \
			HLookDetectorUtil.h \
			HKeypadLookDetector.h \
			HTestingLookDetector.h \
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
#			HVideoImagePlayer.h \
			HNonPlayer.h \
			HVIPlayer.h \
			HStimulusSource.h \
			HImageWidget.h \
			HMediaManager.h \
#			HMainWindow.h \
			HMediaManagerUtil.h \
			HTrialGenerator.h \
#			HOutputGenerator.h \
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
			HReliability.h \
			HStimulusUtil.h \
			habit/HControlPanel.h \
			habit/attentiongettersettings.h \
			habit/attentionsetupform.h \
# 2-24-15 djs			habit/cloneexperimentdialog.h \
			habit/configuration.h \
			habit/connection.h \
			habit/controlbaroptions.h \
			habit/controlbaroptionsform.h \
			habit/criterionsettings.h \
# 3-6-14 djs			habit/designsettings.h \
# 3-6-14 djs			habit/designsetupform.h \
# 2-24-15 djs			habit/experimentresultsform.h \
			habit/experimentsettings.h \
# 11-10-14			habit/experimentsettingsform.h \
#			habit/HExperimentSettingsWidget.h \
# 2-24-15 djs		habit/exportmultipleresults.h \
# 11-10-14			habit/habitapp.h \
#			habit/habitcontrolpanel.h \
			habit/habituationsettings.h \
			habit/habituationsetupform.h \
# 2-24-15 djs			habit/habituationstimuliform.h \
# 2-24-15 djs			habit/habituationstimulitable.h \
# 2-24-15 djs			habit/imagewidget_original.h \
			habit/maindao.h \
#			habit/mediamanager.h \
#			habit/mediaplayer.h \
# 2-24-15 djs			habit/mediasoundpreviewplayer.h \
#			habit/monitormanager.h \
			habit/monitorsettings.h \
# 2-24-15 djs			habit/monitorsettingsform.h \
#			habit/OutputGenerator.h \
# 2-24-15 djs			habit/reliabilitiesform.h \
# 2-24-15 djs			habit/reliabilitysettings.h \
			habit/resource.h \
			habit/resource1.h \
# 2-24-15 djs			habit/resultsvieweroptions.h \
# 2-24-15 djs			habit/resultviewerform.h \
# 2-24-15 djs			habit/resultviewersettings.h \
			habit/runsettings.h \
# 2-24-15 djs			habit/runsettingsform.h \
# 5-19-15 djs			habit/HRunSettingsWidget.h \
			habit/sleeper.h \
			habit/stimulisettings.h \
# 2-24-15 djs			habit/stimulusdisplayform.h \
			habit/stimulusdisplayinfo.h \
			habit/stimulusinfo.h \
			habit/stimulusinfoform.h \
			habit/stimulusplayer.h \
			habit/stimulussettings.h \
#			habit/stimulussettingseditordialog.h \
			habit/subjectinformation.h \
# 2-24-15 djs			habit/subjectinformationwidget.h \
			habit/subjectsettings.h \
			habit/treewizard.h \
			habit/trialsinfo.h \
# 2-24-15 djs			habit/viewexperimentresultform.h \
			habit/HLookSettings.h \
			habit/HLookSettingsWidget.h \
			habit/HPhaseSettings.h \
			habit/HPhaseSettingsWidget.h \
			habit/HOrderSettings.h \
			habit/HOrderSettingsWidget.h \
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
			habit/HResultsExplorerFilesProxyModel.h \
			habit/HMediaStatusWidget.h \
			habit/HExperimentStatusWidget.h \
			habit/HHabituationSetupWidget.h \
			habit/HStimulusOrderListItemDelegate.h \
			habit/HStimulusNameLabelTableModel.h
			

SOURCES +=	HLook.cpp \
			HLooker.cpp \
			HLookDetector.cpp \
			HLookDetectorUtil.cpp \
			HKeypadLookDetector.cpp \
			HTestingLookDetector.cpp \
			HEvents.cpp \
			HState.cpp \
			HStateMachine.cpp \
			HTrial.cpp \
			HPhase.cpp \
			HElapsedTimer.cpp \
			HPlayer.cpp \
			HAudioPlayer.cpp \
#			HVideoImagePlayer.cpp \
			HNonPlayer.cpp \
			HVIPlayer.cpp \
			HStimulusSource.cpp \
			HImageWidget.cpp \
			HMediaManager.cpp \
#			HMainWindow.cpp \
			HMediaManagerUtil.cpp \
			HTrialGenerator.cpp \
#			HOutputGenerator.cpp \
			HPhaseCriteria.cpp \
			HPhaseCriteriaUtil.cpp \
			HDBUtil.cpp \
			HWorkspaceUtil.cpp \
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
			HStimulusUtil.cpp \
			habit/HControlPanel.cpp \
			habit/attentiongettersettings.cpp \
			habit/attentionsetupform.cpp \
#			habit/cloneexperimentdialog.cpp \
			habit/configuration.cpp \
			habit/connection.cpp \
			habit/controlbaroptions.cpp \
			habit/controlbaroptionsform.cpp \
			habit/criterionsettings.cpp \
# 3-6-14			habit/designsettings.cpp \
# 3-6-14 			habit/designsetupform.cpp \
# 2-24-15 djs		habit/experimentresultsform.cpp \
			habit/experimentsettings.cpp \
# 11-10-14			habit/experimentsettingsform.cpp \
#			habit/HExperimentSettingsWidget.cpp \
# 2-24-15 djs			habit/exportmultipleresults.cpp \
# 11-10-14			habit/habitapp.cpp \
#			habit/habitcontrolpanel.cpp \
			habit/habituationsettings.cpp \
			habit/habituationsetupform.cpp \
# 2-24-15 djs			habit/habituationstimuliform.cpp \
# 2-24-15 djs			habit/habituationstimulitable.cpp \
# 2-24-15 djs			habit/imagewidget_original.cpp \
			habit/main.cpp \
			habit/maindao.cpp \
#			habit/mediamanager.cpp \
#			habit/mediaplayer.cpp \
# 2-24-15 djs			habit/mediasoundpreviewplayer.cpp \
			habit/monitormanager.cpp \
			habit/monitorsettings.cpp \
# 2-24-15 djs			habit/monitorsettingsform.cpp \
#			habit/OutputGenerator.cpp \
# 2-24-15 djs			habit/reliabilitiesform.cpp \
# 2-24-15 djs			habit/reliabilitysettings.cpp \
# 2-24-15 djs			habit/resultsvieweroptions.cpp \
# 2-24-15 djs			habit/resultviewerform.cpp \
# 2-24-15 djs			habit/resultviewersettings.cpp \
			habit/runsettings.cpp \
# 2-24-15 djs			habit/runsettingsform.cpp \
# 5-19-15 djs			habit/HRunSettingsWidget.cpp \
			habit/stimulisettings.cpp \
# 2-24-15 djs			habit/stimulusdisplayform.cpp \
			habit/stimulusdisplayinfo.cpp \
			habit/stimulusinfo.cpp \
			habit/stimulusinfoform.cpp \
			habit/stimulusplayer.cpp \
			habit/stimulussettings.cpp \
# 2-24-15 djs			habit/stimulussettingseditordialog.cpp \
			habit/subjectinformation.cpp \
# 2-24-15 djs			habit/subjectinformationwidget.cpp \
			habit/subjectsettings.cpp \
			habit/treewizard.cpp \
			habit/trialsinfo.cpp \
# 2-24-15 djs			habit/viewexperimentresultform.cpp \
			habit/HLookSettings.cpp \
			habit/HLookSettingsWidget.cpp \
			habit/HPhaseSettings.cpp \
			habit/HPhaseSettingsWidget.cpp \
			habit/HOrderSettings.cpp \
			habit/HOrderSettingsWidget.cpp \
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
			habit/HResultsExplorerFilesProxyModel.cpp \
			habit/HMediaStatusWidget.cpp \
			habit/HExperimentStatusWidget.cpp \
			habit/HHabituationSetupWidget.cpp \
			habit/HStimulusOrderListItemDelegate.cpp \
			habit/HStimulusNameLabelTableModel.cpp
			

RESOURCES = habit/resources.qrc

FORMS =		habit/cloneexperimentdialog.ui \
			habit/controlbaroptionsform.ui \
			habit/habitapp.ui \
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
			