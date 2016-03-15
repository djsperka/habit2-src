/*
 * HExperimentMain.cpp
 *
 *  Created on: Sep 8, 2014
 *      Author: Oakeslab
 */

#include "HExperimentMain.h"

#include "monitorsettingsform.h"
#include "controlbaroptionsform.h"
//#include "habituationsetupform.h"
//#include "stimulusdisplayform.h"
#include "HHabituationSetupWidget.h"
#include "attentionsetupform.h"
#include "habituationsettings.h"
//#include "habituationstimuliform.h"
//#include "designsetupform.h"
#include "HLookSettingsWidget.h"
#include "HPhaseSettingsWidget.h"
#include "HStimuliSettingsWidget.h"
#include "HStimulusDisplayInfoWidget.h"
#include "HExperimentTreeWidgetItem.h"


GUILib::HExperimentMain::HExperimentMain(const Habit::ExperimentSettings& experimentSettings, QWidget *parent, bool bReadOnly)
: QDialog(parent)
, m_settings(experimentSettings)
{
	createComponents();
	makeConnections();
	setWindowTitle(QString("Edit Experiment Settings: %1").arg(m_settings.getName()));

	// this dialog is read-only when used to view results, e.g.
	m_pbSave->setDisabled(bReadOnly);
 }

void GUILib::HExperimentMain::makeConnections()
{
	connect(m_pContentsWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
			this, SLOT(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
	connect(m_pbCancel, SIGNAL(clicked()), this, SLOT(cancelButtonClicked()));
	connect(m_pbSave, SIGNAL(clicked()), this, SLOT(saveButtonClicked()));
	connect(m_pbExport, SIGNAL(clicked()), this, SLOT(exportButtonClicked()));

	connect(m_pStimulusDisplayInfoWidget, SIGNAL(stimulusLayoutTypeChanged(int)), m_pPreTestStimuliWidget, SLOT(stimulusLayoutTypeChanged(int)));
	connect(m_pStimulusDisplayInfoWidget, SIGNAL(stimulusLayoutTypeChanged(int)), m_pHabituationStimuliWidget, SLOT(stimulusLayoutTypeChanged(int)));
	connect(m_pStimulusDisplayInfoWidget, SIGNAL(stimulusLayoutTypeChanged(int)), m_pTestStimuliWidget, SLOT(stimulusLayoutTypeChanged(int)));
	connect(m_pStimulusDisplayInfoWidget, SIGNAL(stimulusLayoutTypeChanged(int)), m_pAttentionSetupForm, SLOT(stimulusLayoutTypeChanged(int)));
}

void GUILib::HExperimentMain::cancelButtonClicked()
{
	reject();
}

void GUILib::HExperimentMain::saveButtonClicked()
{
	m_settings = getSettings();
	if(!m_settings.saveToDB())
	{
		QMessageBox::warning(this, "Save failed", "Failed to save experiment settings into database");
	}
	else
	{
		accept();
	}
}

void GUILib::HExperimentMain::exportButtonClicked()
{
	Habit::ExperimentSettings cloned = getSettings().clone(m_settings.getName());
	QDir dir(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation));
	QString tmpfile(dir.absoluteFilePath(QString("%1.hbx").arg(cloned.getName())));
	QString fileName = QFileDialog::getSaveFileName(this, tr("Filename for export"), tmpfile, "Habit Export File (*.hbx)");
	if (!fileName.isNull() && !fileName.isEmpty())
	{
		qDebug() << "Got filename for export: " << fileName;

		// Open file.... will it clobber?
		QFile file(fileName);
		if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
		{
			QDataStream out(&file);
			out << QCoreApplication::instance()->applicationVersion() << cloned;
			file.close();
		}
	}
}


void GUILib::HExperimentMain::currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	Q_UNUSED(previous);
	HExperimentTreeWidgetItem* p = static_cast<HExperimentTreeWidgetItem*>(current);
	m_pPagesWidget->setCurrentIndex(p->stackid());

	// check if we are switching to a stimuli page. if so, then we fetch the current setting for
	// stimulus layout and update the stimuli page to correctly display the stim layout.

	if (p->stackid() == m_stackidPreTestStimuli)
	{
		Habit::ExperimentSettings s = getSettings();
	}
	else if (p->stackid() == m_stackidHabituationStimuli)
	{
		Habit::ExperimentSettings s = getSettings();
	}
	else if (p->stackid() == m_stackidTestStimuli)
	{
		Habit::ExperimentSettings s = getSettings();
	}


}

bool GUILib::HExperimentMain::isModified()
{
	// Get HExperimentSettings from the pages
	Habit::ExperimentSettings settings = getSettings();
	if (!(settings.getAttentionGetterSettings() == m_settings.getAttentionGetterSettings()))
		qDebug() << "AG settings changed";
	if (!(settings.getControlBarOptions() == m_settings.getControlBarOptions()))
		qDebug() << "Control Bar Options changed";
	if (!(settings.getHLookSettings() == m_settings.getHLookSettings()))
		qDebug() << "HLook settings changed";
	if (!(settings.getPreTestPhaseSettings() == m_settings.getPreTestPhaseSettings()))
		qDebug() << "Pretest Phase settings changed";
	if (!(settings.getHabituationPhaseSettings() == m_settings.getHabituationPhaseSettings()))
		qDebug() << "Habituation phase settigns changed";
	if (!(settings.getTestPhaseSettings() == m_settings.getTestPhaseSettings()))
		qDebug() << "Test phase settings chagned";
	if (!(settings.getHabituationSettings() == m_settings.getHabituationSettings()))
		qDebug() << "habituation settings changed";
	if (!(settings.getStimulusDisplayInfo() == m_settings.getStimulusDisplayInfo()))
		qDebug() << "stimulus display info changed";
	if (!(settings.getPreTestStimuliSettings() == m_settings.getPreTestStimuliSettings()))
		qDebug() << "pretest stimuli changed";
	if (!(settings.getHabituationStimuliSettings() == m_settings.getHabituationStimuliSettings()))
		qDebug() << "habituation stimuli changed";
	if (!(settings.getTestStimuliSettings() == m_settings.getTestStimuliSettings()))
		qDebug() << "test stimuli changed";
	if (settings.getId() != m_settings.getId())
		qDebug() << "Id changed";
	if (settings.getName() != m_settings.getName())
		qDebug() << "Name changed";

	return !(getSettings() == m_settings);
}

Habit::ExperimentSettings GUILib::HExperimentMain::getSettings()
{
	Habit::ExperimentSettings settings;
	settings.setId(m_settings.getId());
	settings.setName(m_settings.getName());
	settings.setAttentionGetterSettings(m_pAttentionSetupForm->getConfigurationObject());
	settings.setControlBarOptions(m_pControlBarOptionsForm->getConfigurationObject());
	settings.setHLookSettings(m_pLookSettingsWidget->getHLookSettings());
	settings.setPreTestPhaseSettings(m_pPreTestPhaseWidget->getHPhaseSettings());
	settings.setHabituationPhaseSettings(m_pHabituationPhaseWidget->getHPhaseSettings());
	settings.setTestPhaseSettings(m_pTestPhaseWidget->getHPhaseSettings());
	//settings.setHabituationSettings(m_pHabituationSetupForm->getConfigurationObject());
	settings.setHabituationSettings(m_pHabituationSetupWidget->getHabituationSettings());
	settings.setStimulusDisplayInfo(m_pStimulusDisplayInfoWidget->getStimulusDisplayInfo());
//TODO 	settings.setMonitorSettings(m_pMonitorSettingsForm->getConfigurationObject());

	settings.setHabituationStimuliSettings(m_pHabituationStimuliWidget->getStimuliSettings());
	settings.setPreTestStimuliSettings(m_pPreTestStimuliWidget->getStimuliSettings());
	settings.setTestStimuliSettings(m_pTestStimuliWidget->getStimuliSettings());
	return settings;
}

void GUILib::HExperimentMain::closeEvent(QCloseEvent* event)
{
	if (isModified())
	{
		QMessageBox msgBox;
		msgBox.setText("The settings for this experiment have been modified.");
		msgBox.setInformativeText("Do you want to save your changes?");
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Save);
		int ret = msgBox.exec();


		switch (ret)
		{
			case QMessageBox::Save:
				m_settings = getSettings();
				if(!m_settings.saveToDB())
				{
					QMessageBox::warning(this, "Save failed", "Failed to save experiment settings into database");
					event->ignore();
				}
				else
				{
					event->accept();
				}
				break;
			case QMessageBox::Discard:
				event->accept();
				break;
			case QMessageBox::Cancel:
				event->ignore();
				break;
			default:
				// should never be reached
				break;
		}
	}
	else
	{
		event->accept();
	}
}

void GUILib::HExperimentMain::createComponents()
{
	int iBlank;
	int iStimulusDisplayInfo;
	int iControlBarOptions;
	int iAttentionSetupForm;
	int iLookSettingsWidget;
	int iPreTestPhaseWidget;
	//int iPreTestStimuliWidget;
	int iHabituationPhaseWidget;
	//int iHabituationSetupForm;
	int iHabituationSetupWidget;
	//int iHabituationStimuliWidget;
	int iTestPhaseWidget;
	//int iTestStimuliWidget;


	//m_pContentsWidget = createContentsWidget();
	m_pContentsWidget = new QTreeWidget(this);
	m_pContentsWidget->setColumnCount(1);
	m_pContentsWidget->header()->hide();
	m_pPagesWidget = new QStackedWidget;

	// blank page
	m_pBlank = new QWidget();
	QPalette Pal(palette());
	Pal.setColor(QPalette::Background, Qt::gray);
	m_pBlank->setAutoFillBackground(true);
	m_pBlank->setPalette(Pal);
	iBlank = m_pPagesWidget->addWidget(m_pBlank);

	// Stimulus display info
	m_pStimulusDisplayInfoWidget = new HStimulusDisplayInfoWidget(m_settings.getStimulusDisplayInfo());
	iStimulusDisplayInfo = m_pPagesWidget->addWidget(m_pStimulusDisplayInfoWidget);

	// control bar options
	m_pControlBarOptionsForm = new ControlBarOptionsForm(m_settings.getControlBarOptions());
	iControlBarOptions = m_pPagesWidget->addWidget(m_pControlBarOptionsForm);

	// Attention Getter
	m_pAttentionSetupForm = new AttentionSetupForm(m_settings.getAttentionGetterSettings(), m_settings.getStimulusDisplayInfo());
	iAttentionSetupForm = m_pPagesWidget->addWidget(m_pAttentionSetupForm);

	// Look settings
	m_pLookSettingsWidget = new HLookSettingsWidget(m_settings.getHLookSettings());
	iLookSettingsWidget = m_pPagesWidget->addWidget(m_pLookSettingsWidget);

	// pretest trial settings
	m_pPreTestPhaseWidget = new HPhaseSettingsWidget(m_settings.getPreTestPhaseSettings());
	iPreTestPhaseWidget = m_pPagesWidget->addWidget(m_pPreTestPhaseWidget);

	// pretest stimuli
	m_pPreTestStimuliWidget = new GUILib::HStimuliSettingsWidget(m_settings.getPreTestStimuliSettings(), m_settings.getStimulusDisplayInfo());
	m_stackidPreTestStimuli = m_pPagesWidget->addWidget(m_pPreTestStimuliWidget);

	// habituation trial settings
	m_pHabituationPhaseWidget = new HPhaseSettingsWidget(m_settings.getHabituationPhaseSettings());
	iHabituationPhaseWidget = m_pPagesWidget->addWidget(m_pHabituationPhaseWidget);

	// habituation criteria
	//m_pHabituationSetupForm = new HabituationSetupForm(m_settings.getHabituationSettings());
	//iHabituationSetupForm = m_pPagesWidget->addWidget(m_pHabituationSetupForm);
	m_pHabituationSetupWidget = new GUILib::HHabituationSetupWidget(m_settings.getHabituationSettings());
	iHabituationSetupWidget = m_pPagesWidget->addWidget(m_pHabituationSetupWidget);

	// habituation stimuli
	m_pHabituationStimuliWidget = new GUILib::HStimuliSettingsWidget(m_settings.getHabituationStimuliSettings(), m_settings.getStimulusDisplayInfo());
	m_stackidHabituationStimuli = m_pPagesWidget->addWidget(m_pHabituationStimuliWidget);

	// test trial settings
	m_pTestPhaseWidget = new HPhaseSettingsWidget(m_settings.getTestPhaseSettings());
	iTestPhaseWidget = m_pPagesWidget->addWidget(m_pTestPhaseWidget);

	// test stimuli
	m_pTestStimuliWidget = new GUILib::HStimuliSettingsWidget(m_settings.getTestStimuliSettings(), m_settings.getStimulusDisplayInfo());
	m_stackidTestStimuli = m_pPagesWidget->addWidget(m_pTestStimuliWidget);


	// Build the General items
	HExperimentTreeWidgetItem* ptwiGeneral = new HExperimentTreeWidgetItem(m_pContentsWidget, iBlank, "General");
	//HExperimentTreeWidgetItem* ptwiStimulusDisplayInfo =
	new HExperimentTreeWidgetItem(ptwiGeneral, iStimulusDisplayInfo, "Stimulus Display");
	//HExperimentTreeWidgetItem* ptwiControlBar =
	new HExperimentTreeWidgetItem(ptwiGeneral, iControlBarOptions, "Control Panel Display Options");
	//HExperimentTreeWidgetItem* ptwiAttentionGetter =
	new HExperimentTreeWidgetItem(ptwiGeneral, iAttentionSetupForm, "Intertrial Interval");
	//HExperimentTreeWidgetItem* ptwiLookSettings =
	new HExperimentTreeWidgetItem(ptwiGeneral, iLookSettingsWidget, "Look Settings");

	// PreTest stuff
	HExperimentTreeWidgetItem* ptwiPreTest = new HExperimentTreeWidgetItem(m_pContentsWidget, iBlank, "PreTest Phase");
	//HExperimentTreeWidgetItem* ptwiPreTestSettings =
	new HExperimentTreeWidgetItem(ptwiPreTest, iPreTestPhaseWidget, "Trial Settings");
	//HExperimentTreeWidgetItem* ptwiPreTestStimuli =
	new HExperimentTreeWidgetItem(ptwiPreTest, m_stackidPreTestStimuli, "Stimuli");

	// habituation stuff
	HExperimentTreeWidgetItem* ptwiHabituation = new HExperimentTreeWidgetItem(m_pContentsWidget, iBlank, "Habituation Phase");
	//HExperimentTreeWidgetItem* ptwiHabituationTrialSettings =
	new HExperimentTreeWidgetItem(ptwiHabituation, iHabituationPhaseWidget, "Trial Settings");
	//HExperimentTreeWidgetItem* ptwiHabituationStimuli =
	new HExperimentTreeWidgetItem(ptwiHabituation, m_stackidHabituationStimuli, "Stimuli");
	//HExperimentTreeWidgetItem* ptwiHabituationSetup =
	new HExperimentTreeWidgetItem(ptwiHabituation, iHabituationSetupWidget, "Habituation Settings");

	// Test stuff
	HExperimentTreeWidgetItem* ptwiTest = new HExperimentTreeWidgetItem(m_pContentsWidget, iBlank, "Test Phase");
	//HExperimentTreeWidgetItem* ptwiTestSettings =
	new HExperimentTreeWidgetItem(ptwiTest, iTestPhaseWidget, "Trial Settings");
	//HExperimentTreeWidgetItem* ptwiTestStimuli =
	new HExperimentTreeWidgetItem(ptwiTest, m_stackidTestStimuli, "Stimuli");



    QHBoxLayout *horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(m_pContentsWidget);
    horizontalLayout->addWidget(m_pPagesWidget, 1);

    m_pbExport = new QPushButton("Export");
    m_pbCancel = new QPushButton("Cancel");
    m_pbSave = new QPushButton("Save");

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(m_pbExport);
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(m_pbCancel);
    buttonsLayout->addWidget(m_pbSave);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(horizontalLayout);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(12);
	mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    setWindowTitle(QString("Experiment Settings"));


 }


Habit::StimulusSettings GUILib::HExperimentMain::getTestSS()
{
	Habit::StimulusSettings ss;
	Habit::StimulusInfo l, c, r, s;
	ss.setName("Stimulus 1");
	l.setFileName("/Users/Oakeslab/Desktop/stim/convert.jpg");
	l.setVolume(50);
	l.setLoopPlayBack(false);
	ss.setLeftStimulusInfo(l);

	c.setFileName("/Users/Oakeslab/Desktop/stim/husky.jpg");
	c.setVolume(0);
	c.setLoopPlayBack(false);
	ss.setCenterStimulusInfo(c);

	r.setFileName("/Users/Oakeslab/Desktop/stim/multicolor_box_bubble.mov");
	r.setVolume(75);
	r.setLoopPlayBack(true);
	ss.setRightStimulusInfo(r);

	ss.setIndependentSoundInfo(s);

	return ss;
}


