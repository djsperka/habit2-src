/*
 * HExperimentSelectionDialog.cpp
 *
 *  Created on: May 29, 2015
 *      Author: Oakeslab
 */

#ifndef HEXPERIMENTSELECTIONDIALOG_CPP_
#define HEXPERIMENTSELECTIONDIALOG_CPP_

#include "HExperimentSelectionDialog.h"
#include "experimentsettings.h"
#include "HWorkspaceUtil.h"
#include <QtGui>
#include "MMPreviewDialog.h"


using namespace Habit;
using namespace GUILib;


HExperimentSelectionDialog::HExperimentSelectionDialog(QWidget *parent)
: QDialog(parent)
{
	m_pelw = new GUILib::HExperimentListWidget();
	m_plineeditRoot = new QLineEdit();
	m_pbSelect = new QPushButton("Select");

	QVBoxLayout* vbox = new QVBoxLayout(this);
	vbox->addWidget(m_pelw);

	QHBoxLayout *hroot = new QHBoxLayout;
	hroot->addWidget(new QLabel("Stimulus root folder:"));
	hroot->addWidget(m_plineeditRoot, 1);
	hroot->addWidget(m_pbSelect);
	vbox->addLayout(hroot);

	setLayout(vbox);

    connect(m_pelw, SIGNAL(experimentActivated(QString)), this, SLOT(experimentActivated(QString)));
    connect(m_pbSelect, SIGNAL(clicked()), this, SLOT(selectClicked()));

    // dialog should open with no selection; clear it just in case
    m_pelw->clearSelection();

 }


void HExperimentSelectionDialog::experimentActivated(QString expt)
{
	// Get experiment settings
	Habit::ExperimentSettings settings;
	if (Habit::ExperimentSettings::load(settings, expt))
	{
		QDir dirStimRoot;
		habutilGetStimulusRootDir(dirStimRoot);

		// Start up mm test dialog
		MMPreviewDialog* preview = new MMPreviewDialog(settings.getStimulusDisplayInfo(), dirStimRoot, this);
		preview->exec();

#if 0
		QMessageBox mbox;
		mbox.setText(expt);
		mbox.exec();
#endif

	}
}

void HExperimentSelectionDialog::selectClicked()
{
	if (habutilSelectStimulusRootDir())
	{
		m_plineeditRoot->setText(habutilGetStimulusRootDir());
	}
}



#endif /* HEXPERIMENTSELECTIONDIALOG_CPP_ */
