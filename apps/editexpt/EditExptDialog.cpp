/*
 *  EditExptDialog.cpp
 *  editexpt
 *
 *  Created by Oakes Lab on 9/12/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "EditExptDialog.h"
#include "controlbaroptions.h"
#include "designsettings.h"
#include "trialsinfo.h"
#include "stimulusdisplayinfo.h"
#include "HMediaManagerUtil.h"
#include "HTypes.h"

TestExptDialog::TestExptDialog(QWidget* parent) : QDialog(parent)
{
	Habit::MainDao dao;
	m_vecExperiments = dao.getAllExperiments();
	qDebug() << "experiments has " << m_vecExperiments.size();
	
	QVBoxLayout* layout = new QVBoxLayout;
	m_pbDump = new QPushButton("Dump");
	m_pbTestMediaMgr = new QPushButton("Test Media Mgr");
	m_cbExperiments = new QComboBox();
	for (int i=0; i<m_vecExperiments.size(); i++)
	{
		m_cbExperiments->addItem(m_vecExperiments[i].getName());
	}
	layout->addWidget(m_cbExperiments);
	layout->addWidget(m_pbDump);
	layout->addWidget(m_pbTestMediaMgr);
	setLayout(layout);
	connect(m_pbDump, SIGNAL(clicked()), this, SLOT(onDump()));
	connect(m_pbTestMediaMgr, SIGNAL(clicked()), this, SLOT(onTestMediaMgr()));
};


void TestExptDialog::onTestMediaMgr()
{
	Habit::ExperimentSettings es = m_vecExperiments[m_cbExperiments->currentIndex()];
	es.loadFromDB(true);
	qDebug() << "onTestMediaMgr, selected item is " << m_cbExperiments->currentIndex() << " " << es.getName();

	TestMMDialog* pdialog = new TestMMDialog(es, this);
	pdialog->show();
};

TestMMDialog::TestMMDialog(Habit::ExperimentSettings& es, QWidget* parent) : QDialog(parent)
{
	Habit::StimulusSettingsList v1, v2, v3;
	m_psb = new QSpinBox();
	m_psb->setMaximum(20);
	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(m_psb);
	setLayout(layout);
	m_pmm = createMediaManager(es, this, v1, v2, v3);
	
	qDebug("v1 - PreTest");
	for (int i=0; i<v1.size(); i++)
		qDebug("%d", v1.at(i).first);
	qDebug("v2 - Habituation");
	for (int i=0; i<v2.size(); i++)
		qDebug("%d", v2.at(i).first);
	qDebug("v3 - Test");
	for (int i=0; i<v3.size(); i++)
		qDebug("%d", v3.at(i).first);
	
	
	connect(m_psb, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
	connect(this, SIGNAL(stim(int)), m_pmm, SLOT(stim(int)));
}

void TestMMDialog::valueChanged(int new_value)
{
	emit stim(new_value);
}


void TestExptDialog::onDump()
{
	Habit::ExperimentSettings es = m_vecExperiments[m_cbExperiments->currentIndex()];
	es.loadFromDB(true);
	qDebug() << "onGo, selected item is " << m_cbExperiments->currentIndex() << " " << es.getName();
	
	// Monitor settings
	Habit::MonitorSettings ms = es.getMonitorSettings();
	qDebug() << "Monitors: Control " << ms.getControlMonitorIndex() << " Left " << ms.getLeftMonitorIndex() << " Center " << ms.getCenterMonitorIndex() << " Right " << ms.getRightMonitorIndex();
	
	// Control bar settings
	Habit::ControlBarOptions cbo = es.getControlBarOptions();
	qDebug() << "Control Bar: Use Control Bar? " << cbo.isControlBarUsed() << " Current Expt Displayed? " << cbo.isCurrentExperimentDisplayed() << " Current Stim Displayed? " << cbo.isCurrentStimulusDisplayed();
	
	// DEsign settings - experiment design. The Design Settings hold TrialsInfo for each phase of the expt. 
	Habit::DesignSettings ds = es.getDesignSettings();
	Habit::TrialsInfo tiPreTest = ds.getPretestTrialsInfo();
	Habit::TrialsInfo tiHabituation = ds.getHabituationTrialsInfo();
	Habit::TrialsInfo tiTest = ds.getTestTrialsInfo();
	
	dumpTrialsInfo(tiPreTest, "PreTest");
	dumpTrialsInfo(tiHabituation, "Habituation");
	dumpTrialsInfo(tiTest, "Test");
	
	int minLookTimeMS = es.getDesignSettings().getPretestTrialsInfo().getLookTimes() * 100;
	int maxLookAwayTimeMS = es.getDesignSettings().getHabituationTrialsInfo().getLookTimes() * 100;
	int maxNoLookTimeMS = es.getDesignSettings().getTestTrialsInfo().getLookTimes() * 100;
	
	qDebug() << "minLookTimeMS = es.getDesignSettings().getPretestTrialsInfo().getLookTimes() * 100;  " << minLookTimeMS;
	qDebug() << "maxLookAwayTimeMS = es.getDesignSettings().getHabituationTrialsInfo().getLookTimes() * 100 " << maxLookAwayTimeMS;
	qDebug() << "maxNoLookTimeMS = es.getDesignSettings().getTestTrialsInfo().getLookTimes() * 100 " << maxNoLookTimeMS;

	Habit::HabituationSettings hs = es.getHabituationSettings();
	qDebug() << "HabituationSettings::getHabituationType() " << 
	(hs.getHabituationType() == HHabituationType::HHabituationTypeFixedN ? "HHabituationType::HHabituationTypeFixedN" : "") << 
	(hs.getHabituationType() == HHabituationType::HHabituationTypeCriterion ? "HHabituationType::HHabituationTypeCriterion" : "") << 
	(hs.getHabituationType() == HHabituationType::HHabituationTypeTotalLookingTime ? "HHabituationType::HHabituationTypeTotalLookingTime" : "");

	
	qDebug() << "HabituationSettings::getTotalLookLengthToEnd() " << hs.getTotalLookLengthToEnd();
	
	// fetch CriterionSettings
	Habit::CriterionSettings cs = hs.getCriterionSettings();
	qDebug() << "CriterionSettings::getBasis() " << 
	(cs.getBasis() == HCriterionBasisType::HCriterionBasisFirstN ? "HCriterionBasisType::HCriterionBasisFirstN" : "") <<
	(cs.getBasis() == HCriterionBasisType::HCriterionBasisLongestN ? "HCriterionBasisType::HCriterionBasisLongestN" : "");
	
	qDebug() << "CriterionSettings::getPercent() " << cs.getPercent();
	qDebug() << "CriterionSettings::getWindowSize() " << cs.getWindowSize();
	qDebug() << "CriterionSettings::getWindowType() " << 
	(cs.getWindowType() == HCriterionWindowType::HCriterionWindowFixed ? "HCriterionWindowType::HCriterionWindowFixed" : "") <<
	(cs.getWindowType() == HCriterionWindowType::HCriterionWindowSliding ? "HCriterionWindowType::HCriterionWindowSliding" : "");

	Habit::StimulusDisplayInfo sdi = es.getStimulusDisplayInfo();
	
	qDebug() << "Habit::StimulusDisplayInfo::getPresentationStyle() " <<
	(sdi.getPresentationStyle() == Habit::StimulusDisplayInfo::eMonitorDefined ? "Habit::Habit::StimulusDisplayInfo::eMonitorDefined" : "") <<
	(sdi.getPresentationStyle() == Habit::StimulusDisplayInfo::eSingleMovable ? "Habit::Habit::StimulusDisplayInfo::eSingleMovable" : "");

	qDebug() << "Habit::StimulusDisplayInfo::getDisplayType() " << 
	(sdi.getDisplayType() == Habit::StimulusDisplayInfo::eFullScreen ? "Habit::Habit::StimulusDisplayInfo::eFullScreen" : "") <<
	(sdi.getDisplayType() == Habit::StimulusDisplayInfo::eOriginalSize ? "Habit::Habit::StimulusDisplayInfo::eOriginalSize" : "");
	
	qDebug() << "Habit::StimulusDisplayInfo::isOriginalAspectRatioMaintained() " << sdi.isOriginalAspectRatioMaintained();
	qDebug() << "Habit::StimulusDisplayInfo::getBackGroundColor() " << sdi.getBackGroundColor(); 
	
	Habit::AttentionGetterSettings ags = es.getAttentionGetterSettings();
	qDebug() << "Habit::AttentionGetterSettings::isAttentionGetterUsed() " << ags.isAttentionGetterUsed();
	if (ags.isAttentionGetterUsed())
	{
		dumpStimulusSettings(ags.getAttentionGetterStimulus(), "Attention Getter");
	}
	
	// Now look at stimuli for each of the three phases. 
	Habit::StimuliSettings esPreTest = es.getPreTestStimuliSettings();
	Habit::StimuliSettings esHabituation = es.getHabituationStimuliSettings();
	Habit::StimuliSettings esTest = es.getTestStimuliSettings();
	
	dumpStimuliSettings(esPreTest, "PreTest");
	dumpStimuliSettings(esHabituation, "Habituation");
	dumpStimuliSettings(esTest, "Test");
	
};



void TestExptDialog::dumpStimuliSettings(Habit::StimuliSettings ss, const QString& sText)
{
	QString s = "???";
	if (ss.getStimContext() == HStimContext::PreTestPhase)
	{
		s = "PreTest";
	}
	else if (ss.getStimContext() == HStimContext::HabituationPhase)
	{
		s = "Habituation";
	}
	else if (ss.getStimContext() == HStimContext::TestPhase)
	{
		s = "Test";
	}
	else if (ss.getStimContext() == HStimContext::AttentionGetter)
	{
		s = "AttentionGetter";
	}

	qDebug() << endl << "=========================================================" << endl;
	qDebug() << "StimuliSettings context " << s << " " << sText;
	qDebug() << "There are " << ss.getStimuli().size() << " stimuli.";
	for (int i=0; i<ss.getStimuli().size(); i++)
	{
		dumpStimulusSettings(ss.getStimuli()[i], sText);
	}
	qDebug() << endl << "=========================================================" << endl;
}
	
	
	
void TestExptDialog::dumpStimulusSettings(Habit::StimulusSettings s, const QString& sStimType)
{
	qDebug() << endl << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
	qDebug() << "StimulusSettings for " << sStimType;
	qDebug() << "StimulusSettings::getName() " << s.getName() << " id " << s.getId() << " type " << s.getContext()->name();
	qDebug() << "StimulusSettings::isLeftEnabled() " << s.isLeftEnabled();
	if (s.isLeftEnabled()) dumpStimulusInfo(s.getLeftStimulusInfo(), "Left");
	qDebug() << "StimulusSettings::isCenterEnabled() " << s.isCenterEnabled();
	if (s.isCenterEnabled()) dumpStimulusInfo(s.getCenterStimulusInfo(), "Center");
	qDebug() << "StimulusSettings::isRightEnabled() " << s.isRightEnabled();
	if (s.isRightEnabled()) dumpStimulusInfo(s.getRightStimulusInfo(), "Right");
	qDebug() << "StimulusSettings::isIndependentSoundEnabled() " << s.isIndependentSoundEnabled();
	if (s.isIndependentSoundEnabled()) dumpStimulusInfo(s.getIndependentSoundInfo(), "IndependentSound");
	qDebug() << endl << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
}
	
void TestExptDialog::dumpStimulusInfo(Habit::StimulusInfo info, const QString& sWhichStim)
{
	qDebug() << endl << "------------------------------------------" << endl;
	qDebug() << "StimulusInfo for " << sWhichStim;
	qDebug() << "getName() " << info.getName();
	qDebug() << "getFileName() " << info.getFileName();
	qDebug() << "isLoopPlayback() " << info.isLoopPlayBack();
	qDebug() << "getAudioBalance() " << info.getAudioBalance();
	qDebug() << endl << "------------------------------------------" << endl;
}
	
void TestExptDialog::dumpTrialsInfo(Habit::TrialsInfo info, const QString& sTrialsType)
{
	qDebug() << "Trials info " << sTrialsType ;
	qDebug() << "getType() " << (info.getType() == Habit::TrialsInfo::eFixedLength ? "Habit::TrialsInfo::eFixedLength" : "") << (info.getType() == Habit::TrialsInfo::eSubjectControlled? "Habit::TrialsInfo::eSubjectControlled" : "");
	qDebug() << "getLength()" << 100*info.getLength();
	qDebug() << "getNumberOfTrials() " << 100*info.getNumberOfTrials();
}

EditExptDialog::EditExptDialog(QWidget* parent) : QDialog(parent)
{
	QVBoxLayout* layout = new QVBoxLayout;
	m_pbEdit = new QPushButton("Edit");
	m_pbTest = new QPushButton("Test");
	layout->addWidget(m_pbEdit);
	layout->addWidget(m_pbTest);
	setLayout(layout);
	connect(m_pbEdit, SIGNAL(clicked()), this, SLOT(onEdit()));
	connect(m_pbTest, SIGNAL(clicked()), this, SLOT(onTest()));
};

void EditExptDialog::onEdit()
{
	Habit::ExperimentSettings experimentSettings;
	GUILib::ExperimentSettingsForm experimentSettingsForm(experimentSettings);
	experimentSettingsForm.exec();	
};

void EditExptDialog::onTest()
{
	TestExptDialog* dlg = new TestExptDialog(this);
	dlg->exec();
};
