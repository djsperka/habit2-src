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
#include <QMessageBox>
#include <QVBoxLayout>


using namespace Habit;
using namespace GUILib;


HExperimentSelectionDialog::HExperimentSelectionDialog(QWidget *parent)
: QDialog(parent)
{
	m_pelw = new GUILib::HExperimentListWidget();
	QVBoxLayout* vbox = new QVBoxLayout(this);
	vbox->addWidget(m_pelw);
	setLayout(vbox);

    connect(m_pelw, SIGNAL(experimentActivated(QString)), this, SLOT(experimentActivated(QString)));

    // dialog should open with no selection; clear it just in case
    m_pelw->clearSelection();

 }


void HExperimentSelectionDialog::experimentActivated(QString expt)
{
	// Get experiment settings
	Habit::ExperimentSettings settings;
	if (Habit::ExperimentSettings::load(settings, expt))
	{
		// Start up mm test dialog
#if 0
		QMessageBox mbox;
		mbox.setText(expt);
		mbox.exec();
#endif

	}
}

#endif /* HEXPERIMENTSELECTIONDIALOG_CPP_ */
