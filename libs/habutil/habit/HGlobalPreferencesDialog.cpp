/*
 * HGlobalPreferencesDialog.cpp
 *
 *  Created on: Nov 18, 2014
 *      Author: Oakeslab
 */

#include "HGlobalPreferencesDialog.h"
#include "HWorkspaceUtil.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>

GUILib::HGlobalPreferencesDialog::HGlobalPreferencesDialog(QWidget *parent)
: QDialog(parent)
{
	create();
	connections();
	initialize();
}


void GUILib::HGlobalPreferencesDialog::create()
{
	QGroupBox *pgbWorkspace = new QGroupBox("Current workspace");
	QGroupBox *pgbMonitors = new QGroupBox("Monitor assignments");
	QGroupBox *pgbRoot = new QGroupBox("Stimulus root folder");

	// create pushbuttons
	m_pbDone = new QPushButton("Done");
	m_pbCancel = new QPushButton("Cancel");
	m_pbSelect = new QPushButton("Select");
	m_pbSelectWorkspace = new QPushButton("Select");
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
	QHBoxLayout *hroot = new QHBoxLayout;
	hroot->addWidget(new QLabel("Stimulus root folder:"));
	hroot->addWidget(m_plineeditRoot, 1);
	hroot->addWidget(m_pbSelect);
	QVBoxLayout *vroot = new QVBoxLayout;
	vroot->addLayout(hroot);
	vroot->addWidget(m_pcheckDefaultStimRoot);

	pgbRoot->setLayout(vroot);


	m_plineeditWorkspace = new QLineEdit();
	QHBoxLayout *hworkspace = new QHBoxLayout;
	hworkspace->addWidget(new QLabel("Workspace folder:"));
	hworkspace->addWidget(m_plineeditWorkspace, 1);
	hworkspace->addWidget(m_pbSelectWorkspace);

	pgbWorkspace->setLayout(hworkspace);



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
		m_plineeditRoot->setEnabled(false);
	}
	else
	{
		m_plineeditRoot->setText(habutilGetStimulusRootDir());
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
	habutilSetStimulusRootDir(m_plineeditRoot->text());
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
	}
}


void GUILib::HGlobalPreferencesDialog::selectWorkspaceClicked()
{
	if (habutilSelectWorkspace())
	{
		m_plineeditWorkspace->setText(habutilGetWorkspaceDir());
		habutilInitWorkspace();
		emit workspaceChanged();
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
