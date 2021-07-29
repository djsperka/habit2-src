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
#include <QScreen>
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
//	// if workspace is changed, do that now.
//	if (QDir(ui->lbCurrentWorkspace->text()) != QDir(habutilGetWorkspaceDir()))
//	{
//		habutilSetWorkspace(ui->lbCurrentWorkspace->text());
//		Q_EMIT workspaceChanged();
//	}

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
	return QApplication::screens().size();
}

void GUILib::HGlobalPreferencesDialog::connections()
{
	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &HGlobalPreferencesDialog::accepted);
	connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	connect(ui->pbChooseWorkspace, &QPushButton::clicked, this, &HGlobalPreferencesDialog::chooseWorkspaceClicked);
	connect(ui->pbChooseStimulusDir, &QPushButton::clicked, this, &HGlobalPreferencesDialog::chooseStimulusRootDirClicked);
	connect(ui->pbStimulusDirDefault, &QPushButton::clicked, this, &HGlobalPreferencesDialog::defaultStimRootClicked);
	connect(ui->pbIdentify, &QPushButton::clicked, this, &HGlobalPreferencesDialog::identifyClicked);
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
		qDebug() << " selectged dir " << d.absolutePath();

		// is the selected dir a workspace dir?
		if (!habutilIsValidWorkspace(d))
		{
			// First check if its INSIDE a workspace. If it is, issue a warning and return out of here.
			QDir dup(d);
			qDebug() << "Check parent folder: " << dup.absolutePath();
			if (dup.cdUp())
			{
				if (habutilIsValidWorkspace(dup))
				{
					QMessageBox::critical(this, "Create new workspace", "The directory you chose appears to be INSIDE of another workspace. Please choose another folder.");
					return;
				}
			}

			// Now ask whether the user wants a new workspace created.
			QMessageBox::StandardButton btn;
			btn = QMessageBox::question(this, "Create new workspace", "The selected folder is not a workspace, do you want to create a new one?");
			if (btn == QMessageBox::Yes)
			{
				if (!habutilCreateWorkspace(d))
				{
					// TODO link to log file in dialog box
					QMessageBox::warning(this, "Create new workspace", "Could not create new workspace. See log.");
					return;
				}
			}
		}

		// If we're here, a new workspace has been selected (and created, if necessary).
		habutilSetWorkspace(selectedDir);
		Q_EMIT workspaceChanged();

		// update this dialog
		initialize();
	}
}



void GUILib::HGlobalPreferencesDialog::identifyClicked()
{
	for (int i = 0; i < getNumberOfMonitors(); i++)
	{
		QMessageBox box(QMessageBox::Information, "Information", QString("This monitor is identified as 'Monitor %1'").arg(i));
		QSize size = box.sizeHint();
		QRect screen = QApplication::screens().at(i)->geometry();
		QRect wind(screen.x() + (screen.width()-size.width())/2, screen.y() + (screen.height()-size.height())/2, size.width(), size.height());
		box.setGeometry(wind);
		box.setModal(true);
		box.exec();
	}
}

