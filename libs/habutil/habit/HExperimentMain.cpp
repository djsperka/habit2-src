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
#include "PhaseWizardPages.h"
#include "HGMM.h"
#include "HWorkspaceUtil.h"

using namespace Habit;

#include <QtGlobal>
#if QT_VERSION < 0x050000
#include <QDesktopServices>
#else
#include <QStandardPaths>
#endif



namespace GUILib
{

HExperimentMain::HExperimentMain(const Habit::ExperimentSettings& experimentSettings, QWidget *parent, bool bReadOnly)
: QDialog(parent)
, m_settings(experimentSettings)
{
	// reset media manager. Preview widgets will fetch their respective stimuli via the context stim list.
	QDir dir = habutilGetStimulusRootDir();
	HGMM::instance().reset(experimentSettings, dir);

	// set up gui
	components();
	connections();
	setWindowTitle(QString("Edit Experiment Settings: %1").arg(m_settings.getName()));

	// save sdi stuff
	m_oldStimulusDisplayInfo = m_settings.getStimulusDisplayInfo();

	// this dialog is read-only when used to view results, e.g.
	m_pbSave->setDisabled(bReadOnly);
}

HExperimentMain::~HExperimentMain()
{
	qDebug() << "HExperimentMain::~HExperimentMain()";
	HGMM::instance().reset();
}

void HExperimentMain::components()
{
	QStringList slGeneral;
	slGeneral << "Stimulus Display" << "Run-time Control Panel Display" << "Intertrial Interval" << "Look Settings";

	m_pGeneralListView = new QListView(this);
	m_pGeneralListView->setModel(new QStringListModel(slGeneral));
	m_pGeneralListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_pGeneralListView->setCurrentIndex(m_pGeneralListView->model()->index(0, 0));
	m_pGeneralListView->setFixedHeight(m_pGeneralListView->sizeHintForRow(0) * slGeneral.count() + 2*m_pGeneralListView->frameWidth());

	m_pPagesWidget = new QStackedWidget(this);

	// Stimulus display info
	m_pStimulusDisplayInfoWidget = new HStimulusDisplayInfoWidget(m_settings.getStimulusDisplayInfo());
	m_vecStackPages.append(m_sdiPageIndex = m_pPagesWidget->addWidget(m_pStimulusDisplayInfoWidget));

	// control bar options
	m_pControlBarOptionsForm = new ControlBarOptionsForm(m_settings.getControlBarOptions());
	m_vecStackPages.append(m_pPagesWidget->addWidget(m_pControlBarOptionsForm));

	// Attention Getter
	m_pAttentionSetupForm = new AttentionSetupForm(m_settings.getAttentionGetterSettings(), m_settings.getStimulusDisplayInfo());
	m_vecStackPages.append(m_pPagesWidget->addWidget(m_pAttentionSetupForm));

	// Look settings
	m_pLookSettingsWidget = new HLookSettingsWidget(m_settings.getHLookSettings());
	m_vecStackPages.append(m_pPagesWidget->addWidget(m_pLookSettingsWidget));

	// create phase list widget
	QStringList phaseNames = m_settings.getPhaseNames();
	m_pPhaseListWidget = new HPhaseListWidget(phaseNames);
	m_pPhaseListWidget->clearSelection();	// disables tool buttons

	// add phase settings tab widgets...
	QString s;
	foreach(s, phaseNames)
	{
		HPhaseSettingsTabWidget *pTabWidget = new HPhaseSettingsTabWidget(m_settings.phaseAt(s), s, m_settings.getStimulusDisplayInfo());
		m_pPagesWidget->addWidget(pTabWidget);
		connect(pTabWidget, SIGNAL(phaseNameChanged(const QString&)), m_pPhaseListWidget, SLOT(phaseNameChanged(const QString&)));
		connect(pTabWidget, SIGNAL(phaseEnabledClicked(bool)), m_pPhaseListWidget, SLOT(phaseEnabledClicked(bool)));
		//connect(m_pStimulusDisplayInfoWidget, SIGNAL(stimulusLayoutTypeChanged(int)), pTabWidget, SIGNAL(stimulusLayoutTypeChanged(int)));
	}

	QVBoxLayout *vboxContents = new QVBoxLayout;
	vboxContents->addWidget(new QLabel("Experiment Settings"));
	vboxContents->addWidget(m_pGeneralListView);
	vboxContents->addWidget(m_pPhaseListWidget);
	vboxContents->addStretch(1);

    QHBoxLayout *horizontalLayout = new QHBoxLayout;
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

void HExperimentMain::connections()
{
	connect(m_pGeneralListView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(generalListViewItemClicked(const QModelIndex&)));
	connect(m_pbCancel, SIGNAL(clicked()), this, SLOT(cancelButtonClicked()));
	connect(m_pbSave, SIGNAL(clicked()), this, SLOT(saveButtonClicked()));
	connect(m_pbExport, SIGNAL(clicked()), this, SLOT(exportButtonClicked()));

	connect(m_pPhaseListWidget, SIGNAL(phaseListViewItemClicked(const QModelIndex&)), this, SLOT(phaseListViewItemClicked(const QModelIndex&)));
	connect(m_pPhaseListWidget, SIGNAL(addPhase()), this, SLOT(addPhase()));
	connect(m_pPhaseListWidget, SIGNAL(delPhase()), this, SLOT(delPhase()));
	connect(m_pPhaseListWidget, SIGNAL(upPhase()), this, SLOT(upPhase()));
	connect(m_pPhaseListWidget, SIGNAL(downPhase()), this, SLOT(downPhase()));

	//connect(m_pStimulusDisplayInfoWidget, SIGNAL(stimulusLayoutTypeChanged(int)), m_pAttentionSetupForm, SLOT(stimulusLayoutTypeChanged(int)));
	//connect(this, SIGNAL(stimulusDisplayInfoChanged(const Habit::StimulusDisplayInfo&)), m_pAttentionSetupForm, SLOT(stimulusDisplayInfoChanged(const Habit::StimulusDisplayInfo&)));

	// connect stack widget's currentCjhanged to a slot to look for changes on previous page
	// used for checking changes in stimulus displauy info
	//connect(m_pPagesWidget, SIGNAL(currentChanged(int)), this, SLOT(currentStackPageChanged(int)));
}


void HExperimentMain::checkStimulusDisplayInfo()
{
	// TODO do i have to define != as well as ==?
	if (m_oldStimulusDisplayInfo == m_pStimulusDisplayInfoWidget->getStimulusDisplayInfo())
	{
		qDebug() << "No change in sdi.";
	}
	else
	{
		qDebug() << "stimulusDisplayInfoChanged - reset HGMM";
		m_oldStimulusDisplayInfo = m_pStimulusDisplayInfoWidget->getStimulusDisplayInfo();
//		HGMM::instance().

		m_pAttentionSetupForm->stimulusDisplayInfoChanged(m_oldStimulusDisplayInfo);
		// Now tell each of the phase widgets.
		for (int i=0; i<m_pPagesWidget->count()-4; i++)
		{
			QWidget *w = m_pPagesWidget->widget(4+i);
			HPhaseSettingsTabWidget *ptw = dynamic_cast<HPhaseSettingsTabWidget *>(w);
			ptw->stimulusDisplayInfoChanged(m_oldStimulusDisplayInfo);
		}
	}
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
		m_settings.saveToDB();
		accept();
	}
	catch (const HDBException& e)
	{
		QMessageBox::warning(this, "Save failed", "Failed to save experiment settings into database");
		qCritical() << e.what();
	}
}

void HExperimentMain::exportButtonClicked()
{
	Habit::ExperimentSettings cloned = getSettings().clone(m_settings.getName());
#if QT_VERSION >= 0x050000
	QDir dir(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0]);
#else
	QDir dir(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation));
#endif

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

void HExperimentMain::generalListViewItemClicked(const QModelIndex& index)
{
//	qDebug() << "generalItemClicked row " << index.row() << " switch to " << m_vecStackPages.at(index.row());
	// check stimulus display info page.
	// TODO - ONLY CHECK THIS when sdi page is the one we are leaving
	qDebug() << "Current index " << m_pPagesWidget->currentIndex() << " new index " << index.row() << " sdi page index " << m_sdiPageIndex;
	checkStimulusDisplayInfo();
	m_pPhaseListWidget->clearSelection();
	m_pPagesWidget->setCurrentIndex(m_vecStackPages.at(index.row()));
}

void HExperimentMain::phaseListViewItemClicked(const QModelIndex& index)
{
	qDebug() << "Current index " << m_pPagesWidget->currentIndex() << " new index " << index.row() << " sdi page index " << m_sdiPageIndex;
	checkStimulusDisplayInfo();

	m_pGeneralListView->selectionModel()->select(m_pGeneralListView->selectionModel()->selection(), QItemSelectionModel::Deselect);
	m_pPagesWidget->setCurrentIndex(4 + m_pPhaseListWidget->selectedPhaseIndex());
}

void HExperimentMain::addPhase()
{
	PhaseWizard wiz(m_pPhaseListWidget->currentPhaseNames(), this);
	int i = wiz.exec();
	if (i == QDialog::Accepted)
	{
		HPhaseSettings ps = wiz.getHPhaseSettings();
		int insertIndex = m_pPagesWidget->currentIndex();		// insertIndex is index into m_pPagesWidget, subtract 4 to get index in PhaseListWidget
		if (insertIndex < 4) insertIndex = 4;

		// Add page to pagesWidget
		HPhaseSettingsTabWidget *pTabWidget = new HPhaseSettingsTabWidget(ps, ps.getName(), m_settings.getStimulusDisplayInfo());
		m_pPagesWidget->insertWidget(insertIndex, pTabWidget);
		connect(pTabWidget, SIGNAL(phaseNameChanged(const QString&)), m_pPhaseListWidget, SLOT(phaseNameChanged(const QString&)));
		connect(pTabWidget, SIGNAL(phaseEnabledClicked(bool)), m_pPhaseListWidget, SLOT(phaseEnabledClicked(bool)));

		// Add name to PhaseListWidget
		m_pGeneralListView->clearSelection();
		m_pPhaseListWidget->insertPhase(insertIndex-4, ps.getName());
		m_pPhaseListWidget->setCurrentIndex(insertIndex-4);
		m_pPagesWidget->setCurrentIndex(insertIndex);
	}
}

void HExperimentMain::delPhase()
{
	QString sQuestion = QString("Are you sure you want to delete phase \"%1\"?").arg(m_pPhaseListWidget->selectedPhase());
	if (QMessageBox::Yes == QMessageBox::question(this, "Delete phase?", sQuestion, QMessageBox::Yes|QMessageBox::No, QMessageBox::No))
	{
		int iRefocusIndex;
		int iPhaseIndex = m_pPhaseListWidget->selectedPhaseIndex();
		int iToBeRemoved = 4 + iPhaseIndex;
		if (m_pPhaseListWidget->phaseCount() == 1)
			iRefocusIndex = -1;	// refocus in m_pGeneralListView
		else
			if (iPhaseIndex == m_pPhaseListWidget->phaseCount()-1)
				iRefocusIndex = iPhaseIndex-1;
			else
				iRefocusIndex = iPhaseIndex;

		// Now remove widget
		m_pPagesWidget->removeWidget(m_pPagesWidget->widget(iToBeRemoved));

		// remove from StringListModel...
		m_pPhaseListWidget->removePhase(iPhaseIndex);

		// Refocus selection on first page for simplicity.
		m_pPhaseListWidget->clearSelection();
		m_pPagesWidget->setCurrentIndex(0);
		m_pGeneralListView->setCurrentIndex(m_pGeneralListView->model()->index(0, 0));
	}
}

void HExperimentMain::upPhase()
{
	// need to swap the widgets in the QTabWidget
	int iToBeMoved = 4+m_pPhaseListWidget->selectedPhaseIndex();
	QWidget* pWidgetToBeMoved = m_pPagesWidget->widget(iToBeMoved);
	m_pPagesWidget->removeWidget(pWidgetToBeMoved);
	m_pPagesWidget->insertWidget(iToBeMoved-1, pWidgetToBeMoved);
	m_pPagesWidget->setCurrentIndex(iToBeMoved-1);

	// Move the phase name up in the QListWidget
	m_pPhaseListWidget->movePhaseUp();
	m_pPhaseListWidget->phaseClicked();
}

void HExperimentMain::downPhase()
{
	// need to swap the widgets in the QTabWidget
	int iToBeMoved = 4+m_pPhaseListWidget->selectedPhaseIndex();
	QWidget* pWidgetToBeMoved = m_pPagesWidget->widget(iToBeMoved);
	m_pPagesWidget->removeWidget(pWidgetToBeMoved);
	m_pPagesWidget->insertWidget(iToBeMoved+1, pWidgetToBeMoved);
	m_pPagesWidget->setCurrentIndex(iToBeMoved+1);

	// Move the phase name down in the QListWidget
	m_pPhaseListWidget->movePhaseDown();
	m_pPhaseListWidget->phaseClicked();
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
	if (!(settings.getStimulusDisplayInfo() == m_settings.getStimulusDisplayInfo()))
		qDebug() << "stimulus display info changed";
	if (!(settings.phases() == m_settings.phases()))
		qDebug() << "phases changed.";
	if (settings.getId() != m_settings.getId())
		qDebug() << "Id changed";
	if (settings.getName() != m_settings.getName())
		qDebug() << "Name changed";

	return !(getSettings() == m_settings);
}

Habit::ExperimentSettings HExperimentMain::getSettings()
{
	Habit::ExperimentSettings settings;
	settings.setId(m_settings.getId());
	settings.setName(m_settings.getName());
	settings.setAttentionGetterSettings(m_pAttentionSetupForm->getAttentionGetterSettings());
	qDebug() << "HExperimentMain::getSettings - agsettings";
	qDebug() << settings.getAttentionGetterSettings().getAttentionGetterStimulus();
	settings.setControlBarOptions(m_pControlBarOptionsForm->getConfigurationObject());
	settings.setHLookSettings(m_pLookSettingsWidget->getHLookSettings());
	settings.setStimulusDisplayInfo(m_pStimulusDisplayInfoWidget->getStimulusDisplayInfo());
	// Iterate over PhaseSettingsTabWidgets and append phase settings for each. Set the seqno as
	// they are configured.
	for (int i=0; i<m_pPagesWidget->count()-4; i++)
	{
		QWidget *w = m_pPagesWidget->widget(4+i);
		HPhaseSettingsTabWidget *ptw = dynamic_cast<HPhaseSettingsTabWidget *>(w);
		HPhaseSettings ps = ptw->getPhaseSettings();
		ps.setSeqno(i);
		settings.appendPhase(ps);
	}
	return settings;
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
