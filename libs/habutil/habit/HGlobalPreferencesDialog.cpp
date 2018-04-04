/*
 * HGlobalPreferencesDialog.cpp
 *
 *  Created on: Nov 18, 2014
 *      Author: Oakeslab
 */


// 3/1/18 replace pref dialog


//#include "HHabituationSetupWidget.h"
//#include "ui_HHabituationSetupForm.h"
#include "HGlobalPreferencesDialog.h"
#include "ui_GlobalPreferencesDialog.h"
#include "HWorkspaceUtil.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QDir>


using namespace GUILib;
//using namespace Habit;

HGlobalPreferencesDialog::HGlobalPreferencesDialog(QWidget *parent)
: QDialog(parent)
, ui(new Ui::GlobalPreferencesDialog)
{
	ui->setupUi(this);

	// populate combo boxes
	ui->cbControl->addItem(QString("None"), QVariant(-1));
	ui->cbLeft->addItem(QString("None"), QVariant(-1));
	ui->cbCenter->addItem(QString("None"), QVariant(-1));
	ui->cbRight->addItem(QString("None"), QVariant(-1));
	for (int i=0; i<getNumberOfMonitors(); i++)
	{
		ui->cbControl->addItem(QString("Monitor %1").arg(i), QVariant(i));
		ui->cbLeft->addItem(QString("Monitor %1").arg(i), QVariant(i));
		ui->cbCenter->addItem(QString("Monitor %1").arg(i), QVariant(i));
		ui->cbRight->addItem(QString("Monitor %1").arg(i), QVariant(i));
	}


	initialize();
	connections();
}

void GUILib::HGlobalPreferencesDialog::initialize()
{
	m_sWorkspaceSelected = habutilGetWorkspaceDir().absolutePath();
	m_sStimulusRootDirSelected = habutilGetStimulusRootDir().absolutePath();

	ui->lbCurrentWorkspace->setText(m_sWorkspaceSelected);
	ui->lbCurrentStimDir->setText(m_sStimulusRootDirSelected);


	// set current index on combos
	ui->cbControl->setCurrentIndex(ui->cbControl->findData(QVariant(habutilGetMonitorID(HPlayerPositionType::Control))));
	ui->cbLeft->setCurrentIndex(ui->cbLeft->findData(QVariant(habutilGetMonitorID(HPlayerPositionType::Left))));
	ui->cbCenter->setCurrentIndex(ui->cbCenter->findData(QVariant(habutilGetMonitorID(HPlayerPositionType::Center))));
	ui->cbRight->setCurrentIndex(ui->cbRight->findData(QVariant(habutilGetMonitorID(HPlayerPositionType::Right))));
}

void GUILib::HGlobalPreferencesDialog::accepted()
{
	// if workspace is changed, do that now.
	if (QDir(ui->lbCurrentWorkspace->text()) != QDir(habutilGetWorkspaceDir()))
	{
		habutilSetWorkspace(ui->lbCurrentWorkspace->text());
		Q_EMIT workspaceChanged();
	}

	// set preferences
	habutilSetStimulusRootDir(ui->lbCurrentStimDir->text());
	habutilSetMonitorID(HPlayerPositionType::Control, ui->cbControl->itemData(ui->cbControl->currentIndex()).toInt());
	habutilSetMonitorID(HPlayerPositionType::Left, ui->cbLeft->itemData(ui->cbLeft->currentIndex()).toInt());
	habutilSetMonitorID(HPlayerPositionType::Center, ui->cbCenter->itemData(ui->cbCenter->currentIndex()).toInt());
	habutilSetMonitorID(HPlayerPositionType::Right, ui->cbRight->itemData(ui->cbRight->currentIndex()).toInt());
	habutilSaveSettings();
	accept();
}

void GUILib::HGlobalPreferencesDialog::chooseStimulusRootDirClicked()
{
	QString selectedDir;
	selectedDir = QFileDialog::getExistingDirectory(0, "Select Habit Stimulus Root Folder", ui->lbCurrentStimDir->text());

	// check whether user cancelled or not.
	if (!selectedDir.isEmpty())
	{
		// store the dir in the label
		ui->lbCurrentStimDir->setText(selectedDir);
	}
}

int GUILib::HGlobalPreferencesDialog::getNumberOfMonitors()
{
	return QApplication::desktop()->screenCount();
}

void GUILib::HGlobalPreferencesDialog::connections()
{
	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &HGlobalPreferencesDialog::accepted);
	connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	connect(ui->pbChooseWorkspace, &QPushButton::clicked, this, &HGlobalPreferencesDialog::chooseWorkspaceClicked);
	connect(ui->pbChooseStimulusDir, &QPushButton::clicked, this, &HGlobalPreferencesDialog::chooseStimulusRootDirClicked);
	connect(ui->pbStimulusDirDefault, &QPushButton::clicked, this, &HGlobalPreferencesDialog::defaultStimRootClicked);
}

void GUILib::HGlobalPreferencesDialog::defaultStimRootClicked()
{
	ui->lbCurrentStimDir->setText(ui->lbCurrentWorkspace->text().append("/stim"));
}

void GUILib::HGlobalPreferencesDialog::chooseWorkspaceClicked()
{
	//
	QString selectedDir;
	selectedDir = QFileDialog::getExistingDirectory(0, "Select Workspace Folder", ui->lbCurrentWorkspace->text());

	// check whether user cancelled or not.
	if (!selectedDir.isEmpty())
	{
		QDir d(selectedDir);

		// is the selected dir a workspace dir?
		if (habutilIsValidWorkspace(d))
		{
			ui->lbCurrentWorkspace->setText(selectedDir);
		}
		else
		{
			// TODO: not a workspace - want a new one? First check if its INSIDE a workspace. or if it contains a workspace

			QMessageBox::StandardButton btn;
			btn = QMessageBox::question(this, "Create new workspace", "The selected folder is not a workspace, do you want to create a new one?");
			if (btn == QMessageBox::Yes)
			{
				if (!habutilCreateWorkspace(d))
				{
					// TODO link to log file in dialog box
					QMessageBox::warning(this, "Create new workspace", "Could not create new workspace. See log.");

					// don't save selected in label -- nothing is changed
				}
				else
				{
					ui->lbCurrentWorkspace->setText(selectedDir);
				}
			}
		}
	}
}


void GUILib::HGlobalPreferencesDialog::identifyClicked()
{
	for (int i = 0; i < getNumberOfMonitors(); i++)
	{
		QMessageBox box(QMessageBox::Information, "Information", QString("This monitor is identified as 'Monitor %1'").arg(i));
		QSize size = box.sizeHint();
		QRect screen = QApplication::desktop()->screenGeometry(i);
		QRect wind(screen.x() + (screen.width()-size.width())/2, screen.y() + (screen.height()-size.height())/2, size.width(), size.height());
		box.setGeometry(wind);
		box.setModal(true);
		box.exec();
	}
}


#if 0
HHabituationSetupWidget::HHabituationSetupWidget(QWidget* w)
: QWidget(w)
, ui(new Ui::HHabituationSetupForm)
, m_settings()
{
	ui->setupUi(this);
	initializeComponents();
	connections();
	initialize();
}

#endif


#if 0
#include "HGlobalPreferencesDialog.h"
#include "HWorkspaceUtil.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QDir>


GUILib::HGlobalPreferencesDialog::HGlobalPreferencesDialog(QWidget *parent)
: QDialog(parent)
{
	create();
	connections();
	initialize();
}


void GUILib::HGlobalPreferencesDialog::create()
{
	QGroupBox *pgbWorkspace = new QGroupBox("Folde assignments");
	QGroupBox *pgbMonitors = new QGroupBox("Monitor assignments");
	QGroupBox *pgbRoot = new QGroupBox("Stimulus root folder");

	// create labels
	m_plbWorkspaceDir = new QLabel(habutilGetWorkspaceDir());
	m_plbStimulusRootDir = new QLabel(habutilGetStimulusRootDir());

	// create pushbuttons
	m_pbDone = new QPushButton("Done");
	m_pbCancel = new QPushButton("Cancel");
	m_pbSelect = new QPushButton("Select");
	m_pbSelectWorkspace = new QPushButton("Choose...");
	m_pbIdentify = new QPushButton("Identify Monitors");

	m_pcbControl = new QComboBox();
	m_pcbLeft = new QComboBox();
	m_pcbCenter = new QComboBox();
	m_pcbRight = new QComboBox();
	m_pcheckDefaultStimRoot = new QCheckBox("Use default stimulus root dir");

	m_pcbControl->addItem(QString("None"), QVariant(-1));
	m_pcbLeft->addItem(QString("None"), QVariant(-1));
	m_pcbCenter->addItem(QString("None"), QVariant(-1));
	m_pcbRight->addItem(QString("None"), QVariant(-1));
	for (int i=0; i<getNumberOfMonitors(); i++)
	{
		m_pcbControl->addItem(QString("Monitor %1").arg(i), QVariant(i));
		m_pcbLeft->addItem(QString("Monitor %1").arg(i), QVariant(i));
		m_pcbCenter->addItem(QString("Monitor %1").arg(i), QVariant(i));
		m_pcbRight->addItem(QString("Monitor %1").arg(i), QVariant(i));
	}
	QHBoxLayout *h1 = new QHBoxLayout;
	h1->addStretch(1);
	h1->addWidget(new QLabel("Control"));
	h1->addWidget(m_pcbControl);
	h1->addStretch(1);

	QHBoxLayout *h2 = new QHBoxLayout;
	h2->addStretch(1);
	h2->addWidget(new QLabel("Center"));
	h2->addWidget(m_pcbCenter);
	h2->addStretch(1);

	QHBoxLayout *h3 = new QHBoxLayout;
	h3->addStretch(1);
	h3->addWidget(new QLabel("Left"));
	h3->addWidget(m_pcbLeft);
	h3->addStretch(1);
	h3->addWidget(new QLabel("Right"));
	h3->addWidget(m_pcbRight);
	h3->addStretch(1);

	QHBoxLayout *h4 = new QHBoxLayout;
	h4->addStretch(1);
	h4->addWidget(m_pbIdentify);
	h4->addStretch(1);

	QVBoxLayout *v = new QVBoxLayout;
	v->addLayout(h1);
	v->addLayout(h2);
	v->addLayout(h3);
	v->addLayout(h4);

	pgbMonitors->setLayout(v);

	m_plineeditRoot = new QLineEdit();
	//QHBoxLayout *hroot = new QHBoxLayout;
	QVBoxLayout *vroot = new QVBoxLayout;
	vroot->addWidget(new QLabel("Stimulus root folder:"));
	vroot->addWidget(m_plineeditRoot, 1);
	vroot->addWidget(m_plbStimulusRootDir);
	vroot->addWidget(m_pbSelect);
	QVBoxLayout *vvroot = new QVBoxLayout;
	vvroot->addLayout(vroot);
	vvroot->addWidget(m_pcheckDefaultStimRoot);

	pgbRoot->setLayout(vvroot);


	m_plineeditWorkspace = new QLineEdit();
	QVBoxLayout *vworkspace = new QVBoxLayout;
	vworkspace->addWidget(new QLabel("Workspace folder:"));
	vworkspace->addWidget(m_plineeditWorkspace, 1);
	vworkspace->addWidget(m_plbWorkspaceDir);
	vworkspace->addWidget(m_pbSelectWorkspace);

	pgbWorkspace->setLayout(vworkspace);



	QHBoxLayout* hh = new QHBoxLayout;
	hh->addStretch(1);
	hh->addWidget(m_pbCancel);
	hh->addWidget(m_pbDone);

	QVBoxLayout *vv = new QVBoxLayout;
	vv->addWidget(pgbWorkspace);
	vv->addWidget(pgbMonitors);
	vv->addWidget(pgbRoot);
	vv->addStretch(1);
	vv->addLayout(hh);

	setLayout(vv);
}

int GUILib::HGlobalPreferencesDialog::getNumberOfMonitors()
{
	return QApplication::desktop()->screenCount();
}

void GUILib::HGlobalPreferencesDialog::connections()
{
	connect(m_pbDone, SIGNAL(clicked()), this, SLOT(doneClicked()));
	connect(m_pbCancel, SIGNAL(clicked()), this, SLOT(reject()));
	connect(m_pbSelect, SIGNAL(clicked()), this, SLOT(selectClicked()));
	connect(m_pbSelectWorkspace, SIGNAL(clicked()), this, SLOT(selectWorkspaceClicked()));
	connect(m_pbIdentify, SIGNAL(clicked()), this, SLOT(identifyClicked()));
	connect(m_pcheckDefaultStimRoot, SIGNAL(clicked()), this, SLOT(defaultStimRootClicked()));
}

void GUILib::HGlobalPreferencesDialog::defaultStimRootClicked()
{
	if (m_pcheckDefaultStimRoot->isChecked())
	{
		m_plineeditRoot->setText("<workspace_dir>/stim");
		m_plbStimulusRootDir->setText("<workspace_dir>/stim");
		m_plineeditRoot->setEnabled(false);
	}
	else
	{
		m_plineeditRoot->setText(habutilGetStimulusRootDir());
		m_plbStimulusRootDir->setText(habutilGetStimulusRootDir());
		m_plineeditRoot->setEnabled(true);
	}
}

void GUILib::HGlobalPreferencesDialog::initialize()
{
	m_plineeditWorkspace->setText(habutilGetWorkspaceDir());
	m_pcbControl->setCurrentIndex(m_pcbControl->findData(QVariant(habutilGetMonitorID(HPlayerPositionType::Control))));
	m_pcbLeft->setCurrentIndex(m_pcbLeft->findData(QVariant(habutilGetMonitorID(HPlayerPositionType::Left))));
	m_pcbCenter->setCurrentIndex(m_pcbCenter->findData(QVariant(habutilGetMonitorID(HPlayerPositionType::Center))));
	m_pcbRight->setCurrentIndex(m_pcbRight->findData(QVariant(habutilGetMonitorID(HPlayerPositionType::Right))));

	if (habutilGetUseDefaultStimRoot())
	{
		m_pcheckDefaultStimRoot->setChecked(true);
	}
	else
	{
		m_pcheckDefaultStimRoot->setChecked(false);
	}
	defaultStimRootClicked();
}

void GUILib::HGlobalPreferencesDialog::doneClicked()
{
	// set preferences
	habutilSetStimulusRootDir(m_plbStimulusRootDir->text());
	habutilSetMonitorID(HPlayerPositionType::Control, m_pcbControl->itemData(m_pcbControl->currentIndex()).toInt());
	habutilSetMonitorID(HPlayerPositionType::Left, m_pcbLeft->itemData(m_pcbLeft->currentIndex()).toInt());
	habutilSetMonitorID(HPlayerPositionType::Center, m_pcbCenter->itemData(m_pcbCenter->currentIndex()).toInt());
	habutilSetMonitorID(HPlayerPositionType::Right, m_pcbRight->itemData(m_pcbRight->currentIndex()).toInt());
	habutilSetUseDefaultStimRoot(m_pcheckDefaultStimRoot->isChecked());
	accept();
}

void GUILib::HGlobalPreferencesDialog::selectClicked()
{
	if (habutilSelectStimulusRootDir())
	{
		m_plineeditRoot->setText(habutilGetStimulusRootDir());
		m_plbStimulusRootDir->setText(habutilGetStimulusRootDir());
	}
}


// TODO WORKSPACE SELECTIUON APPLIED IMMEDIATELY, BUT STIM ROOT IS NOT. MAKE WORKSPACE EFFECTIVE IN DONE.

void GUILib::HGlobalPreferencesDialog::selectWorkspaceClicked()
{
	if (habutilSelectWorkspace())
	{
		m_plineeditWorkspace->setText(habutilGetWorkspaceDir());
		m_plbWorkspaceDir->setText(habutilGetWorkspaceDir());
	}
}


void GUILib::HGlobalPreferencesDialog::identifyClicked()
{
	for (int i = 0; i < getNumberOfMonitors(); i++)
	{
		QMessageBox box(QMessageBox::Information, "Information", QString("This monitor is identified as 'Monitor %1'").arg(i));
		QSize size = box.sizeHint();
		QRect screen = QApplication::desktop()->screenGeometry(i);
		QRect wind(screen.x() + (screen.width()-size.width())/2, screen.y() + (screen.height()-size.height())/2, size.width(), size.height());
		box.setGeometry(wind);
		box.setModal(true);
		box.exec();
	}
}

int GUILib::HGlobalPreferencesDialog::getMonitorID(const HPlayerPositionType& type) const
{
	int id = 0;	// monitor id are indexes, numbered from 0, into the contents of QApplication::desktop()

	if (type == HPlayerPositionType::Control)
	{
		id = m_pcbControl->currentIndex();
	}
	else if (type == HPlayerPositionType::Left)
	{
		id = m_pcbLeft->currentIndex();
	}
	else if (type == HPlayerPositionType::Center)
	{
		id = m_pcbCenter->currentIndex();
	}
	else if (type == HPlayerPositionType::Right)
	{
		id = m_pcbRight->currentIndex();
	}

	return id;
}

QString GUILib::HGlobalPreferencesDialog::getRootFolder() const
{
	return m_plineeditRoot->text();
}

#endif
