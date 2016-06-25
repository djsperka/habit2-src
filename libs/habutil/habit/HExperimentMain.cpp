/*
 * HExperimentMain.cpp
 *
 *  Created on: Sep 8, 2014
 *      Author: Oakeslab
 */

#include "HExperimentMain.h"

//#include "monitorsettingsform.h"
#include "controlbaroptionsform.h"
#include "HHabituationSetupWidget.h"
#include "attentionsetupform.h"
#include "habituationsettings.h"
#include "HLookSettingsWidget.h"
#include "HPhaseSettingsTabWidget.h"
#include "HStimulusDisplayInfoWidget.h"
#include "HExperimentTreeWidgetItem.h"
#include "HDBException.h"

using namespace Habit;

namespace GUILib
{

HExperimentMain::HExperimentMain(const Habit::ExperimentSettings& experimentSettings, QWidget *parent, bool bReadOnly)
: QDialog(parent)
, m_settings(experimentSettings)
{
	createComponents();
	makeConnections();
	setWindowTitle(QString("Edit Experiment Settings: %1").arg(m_settings.getName()));

	// this dialog is read-only when used to view results, e.g.
	m_pbSave->setDisabled(bReadOnly);
 }

void HExperimentMain::makeConnections()
{
//	connect(m_pContentsWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
//			this, SLOT(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
	connect(m_pGeneralListView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(generalItemClicked(const QModelIndex&)));
	connect(m_pbCancel, SIGNAL(clicked()), this, SLOT(cancelButtonClicked()));
	connect(m_pbSave, SIGNAL(clicked()), this, SLOT(saveButtonClicked()));
	connect(m_pbExport, SIGNAL(clicked()), this, SLOT(exportButtonClicked()));

	qCritical() << "RE-CONNECT to stimulus layout changes. See HExperimentMain::makeConnections()";
	/*
	connect(m_pStimulusDisplayInfoWidget, SIGNAL(stimulusLayoutTypeChanged(int)), m_pPreTestStimuliWidget, SLOT(stimulusLayoutTypeChanged(int)));
	connect(m_pStimulusDisplayInfoWidget, SIGNAL(stimulusLayoutTypeChanged(int)), m_pHabituationStimuliWidget, SLOT(stimulusLayoutTypeChanged(int)));
	connect(m_pStimulusDisplayInfoWidget, SIGNAL(stimulusLayoutTypeChanged(int)), m_pTestStimuliWidget, SLOT(stimulusLayoutTypeChanged(int)));
	connect(m_pStimulusDisplayInfoWidget, SIGNAL(stimulusLayoutTypeChanged(int)), m_pAttentionSetupForm, SLOT(stimulusLayoutTypeChanged(int)));
	*/
}

void HExperimentMain::cancelButtonClicked()
{
	reject();
}

void HExperimentMain::saveButtonClicked()
{
	m_settings = getSettings();
	try
	{
		//TODO transation()
		m_settings.saveToDB();
		//TODO commit()
		accept();
	}
	catch (const HDBException& e)
	{
		//TODO rollback()
		QMessageBox::warning(this, "Save failed", "Failed to save experiment settings into database");
	}
}

void HExperimentMain::exportButtonClicked()
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

void HExperimentMain::generalItemClicked(const QModelIndex& index)
{
//	qDebug() << "generalItemClicked row " << index.row() << " switch to " << m_vecStackPages.at(index.row());
	m_pPagesWidget->setCurrentIndex(m_vecStackPages.at(index.row()));
}

void HExperimentMain::currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	Q_UNUSED(previous);
	HExperimentTreeWidgetItem* p = static_cast<HExperimentTreeWidgetItem*>(current);
	m_pPagesWidget->setCurrentIndex(p->stackid());

	// If the currently selected item is a phase, enable the add/del buttons,
	// otherwise disable them

	if (p->isPhase())
	{
		//m_actionNewPhase->setEnabled(true);
		//if (p->childCount() == 0)
			//m_actionDelPhase->setEnabled(true);
		//else
			//m_actionDelPhase->setEnabled(false);
	}
	else
	{
		//m_actionNewPhase->setEnabled(false);
		//ßm_actionDelPhase->setEnabled(false);
	}


}

bool HExperimentMain::isModified()
{
	// Get HExperimentSettings from the pages
	Habit::ExperimentSettings settings = getSettings();
	if (!(settings.getAttentionGetterSettings() == m_settings.getAttentionGetterSettings()))
		qDebug() << "AG settings changed";
	if (!(settings.getControlBarOptions() == m_settings.getControlBarOptions()))
		qDebug() << "Control Bar Options changed";
	if (!(settings.getHLookSettings() == m_settings.getHLookSettings()))
		qDebug() << "HLook settings changed";

	// TODO: testing of phases must account for deleted/added phases, stimuli.
#if 0
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
#else
	qWarning() << "Not testing equality of phases/stimuli!";
#endif

	if (settings.getId() != m_settings.getId())
		qDebug() << "Id changed";
	if (settings.getName() != m_settings.getName())
		qDebug() << "Name changed";

	return !(getSettings() == m_settings);
}

Habit::ExperimentSettings HExperimentMain::getSettings()
{

	//TODO Fix getSettings in HExperimentMain!!!!
	QMessageBox::warning(this, "Bad Move", "Cannot save settings.");
	return m_settings;

#if 0
	Habit::ExperimentSettings settings;
	settings.setId(m_settings.getId());
	settings.setName(m_settings.getName());
	settings.setAttentionGetterSettings(m_pAttentionSetupForm->getConfigurationObject());
	settings.setControlBarOptions(m_pControlBarOptionsForm->getConfigurationObject());
	settings.setHLookSettings(m_pLookSettingsWidget->getHLookSettings());
	settings.setPreTestPhaseSettings(m_pPreTestPhaseWidget->getHPhaseSettings());
	settings.setHabituationPhaseSettings(m_pHabituationPhaseWidget->getHPhaseSettings());
	settings.setTestPhaseSettings(m_pTestPhaseWidget->getHPhaseSettings());
	settings.setHabituationSettings(m_pHabituationSetupWidget->getHabituationSettings());
	settings.setStimulusDisplayInfo(m_pStimulusDisplayInfoWidget->getStimulusDisplayInfo());

	settings.setHabituationStimuliSettings(m_pHabituationStimuliWidget->getStimuliSettings());
	settings.setPreTestStimuliSettings(m_pPreTestStimuliWidget->getStimuliSettings());
	settings.setTestStimuliSettings(m_pTestStimuliWidget->getStimuliSettings());
	return settings;
#endif
}

void HExperimentMain::closeEvent(QCloseEvent* event)
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
				try
				{
					m_settings.saveToDB();
					event->accept();
				}
				catch (const HDBException& e)
				{
					QMessageBox::warning(this, "Save failed", e.what());
					event->ignore();
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

void HExperimentMain::createComponents()
{
	//int iBlank;
	//int iStimulusDisplayInfo;
	//int iControlBarOptions;
	//int iAttentionSetupForm;
	//int iLookSettingsWidget;
	QStringList slGeneral;
	slGeneral << "Stimulus Display" << "Run-time Control Panel Display" << "Intertrial Interval" << "Look Settings";

	m_pGeneralListView = new QListView(this);
	m_pGeneralListView->setModel(new QStringListModel(slGeneral));
	m_pGeneralListView->setCurrentIndex(m_pGeneralListView->model()->index(0, 0));
	m_pGeneralListView->setFixedHeight(m_pGeneralListView->sizeHintForRow(0) * slGeneral.count() + 2*m_pGeneralListView->frameWidth());

//	m_pContentsWidget = new QTreeWidget(this);
//	m_pContentsWidget->setColumnCount(1);
//	m_pContentsWidget->header()->hide();
	m_pPagesWidget = new QStackedWidget;

	// blank page
	//m_pBlank = new QWidget();
	//QPalette Pal(palette());
	//Pal.setColor(QPalette::Background, Qt::gray);
	//m_pBlank->setAutoFillBackground(true);
	//m_pBlank->setPalette(Pal);
	//iBlank = m_pPagesWidget->addWidget(m_pBlank);

	// Stimulus display info
	m_pStimulusDisplayInfoWidget = new HStimulusDisplayInfoWidget(m_settings.getStimulusDisplayInfo());
	m_vecStackPages.append(m_pPagesWidget->addWidget(m_pStimulusDisplayInfoWidget));

	// control bar options
	m_pControlBarOptionsForm = new ControlBarOptionsForm(m_settings.getControlBarOptions());
	m_vecStackPages.append(m_pPagesWidget->addWidget(m_pControlBarOptionsForm));

	// Attention Getter
	m_pAttentionSetupForm = new AttentionSetupForm(m_settings.getAttentionGetterSettings(), m_settings.getStimulusDisplayInfo());
	m_vecStackPages.append(m_pPagesWidget->addWidget(m_pAttentionSetupForm));

	// Look settings
	m_pLookSettingsWidget = new HLookSettingsWidget(m_settings.getHLookSettings());
	m_vecStackPages.append(m_pPagesWidget->addWidget(m_pLookSettingsWidget));

#if 0
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
#endif

#if 0
	// Now build phase stuff
	HExperimentTreeWidgetItem* ptwiPhases = new HExperimentTreeWidgetItem(m_pContentsWidget, iBlank, "Phases", true);
	QListIterator<HPhaseSettings> phaseIterator = m_settings.phaseIterator();
	while (phaseIterator.hasNext())
	{
		const Habit::HPhaseSettings& ps = phaseIterator.next();

		// create phase settings widget and add to stack widget
		//HPhaseSettingsWidget* pPhaseSettingsWidget = new HPhaseSettingsWidget(ps);
		//int item = m_pPagesWidget->addWidget(pPhaseSettingsWidget);
		HPhaseSettingsTabWidget* pPhaseSettingsTabWidget = new HPhaseSettingsTabWidget(ps, ps.getName(), m_settings.getStimulusDisplayInfo());
		int item = m_pPagesWidget->addWidget(pPhaseSettingsTabWidget);

		new HExperimentTreeWidgetItem(ptwiPhases, item, ps.getName(), true);
	}

	m_pPhaseToolBar = new QToolBar();
	m_actionNewPhase = new QAction(QIcon(":/resources/plus.png"), "Add new phase", this);
	m_actionDelPhase = new QAction(QIcon(":/resources/delete.png"), "Delete selected phase", this);
	m_pPhaseToolBar->addAction(m_actionNewPhase);
	connect(m_actionNewPhase, SIGNAL(triggered()), this, SLOT(addPhase()));
	m_pPhaseToolBar->addAction(m_actionDelPhase);
	connect(m_actionDelPhase, SIGNAL(triggered()), this, SLOT(delPhase()));

	QVBoxLayout *vboxContents = new QVBoxLayout;
	vboxContents->addWidget(m_pContentsWidget);
	vboxContents->addWidget(m_pPhaseToolBar);
#else

	m_pPhaseListWidget = new HPhaseListWidget(m_settings.getPhaseNames());


	QVBoxLayout *vboxContents = new QVBoxLayout;
//	vboxContents->addWidget(m_pContentsWidget);
	vboxContents->addWidget(new QLabel("Experiment Settings"));
	vboxContents->addWidget(m_pGeneralListView);
	vboxContents->addWidget(m_pPhaseListWidget);
	vboxContents->addStretch(1);

#endif
    QHBoxLayout *horizontalLayout = new QHBoxLayout;
    //horizontalLayout->addWidget(m_pContentsWidget);
    horizontalLayout->addLayout(vboxContents);
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

Habit::StimulusSettings HExperimentMain::getTestSS()
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


}; /* namespace GUILib */
