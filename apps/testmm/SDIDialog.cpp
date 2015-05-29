/*
 * SDIDialog.cpp
 *
 *  Created on: May 20, 2015
 *      Author: Oakeslab
 */

#include "SDIDialog.h"
using namespace GUILib;

#include "stimulusdisplayinfo.h"
using namespace Habit;

#include "HMediaManager.h"
#include "HVIPlayer.h"
#include "HAudioPlayer.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QtDebug>
#include <QPushButton>
#include <QWidget>

SDIDialog::SDIDialog(QWidget* w)
: QDialog(w)
{
	StimulusDisplayInfo sdi;
	QVBoxLayout *box = new QVBoxLayout;

	m_psdiw = new HStimulusDisplayInfoWidget(sdi, NULL);
	box->addWidget(m_psdiw);


	QPushButton* showButton = new QPushButton(tr("ShowMM"));
    showButton->setDefault(true);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	buttonBox->addButton(showButton, QDialogButtonBox::ActionRole);

    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(showButton, SIGNAL(clicked()), this, SLOT(showClicked()));

    box->addWidget(buttonBox);
	setLayout(box);
}

SDIDialog::~SDIDialog()
{
}


void SDIDialog::showClicked()
{
	QDialog dlg;

	// Create MM, dislplay
	qDebug() << "show clicked";

	m_pmm = new HMediaManager(false);

	// Create a widget to hold the players.
	QWidget *pwidget = new QWidget(this);
	QHBoxLayout *hbox = new QHBoxLayout;

	StimulusDisplayInfo info(m_psdiw->getStimulusDisplayInfo());

	if (info.getUseISS())
	{
		HPlayer *p = new HAudioPlayer(-1, NULL, QDir());
		m_pmm->addPlayer(HPlayerPositionType::Sound, p, -1);
		//hbox->addWidget(p);
	}
	if (info.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		HPlayer *p = new HVIPlayer(-1, NULL, QDir(), info.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, info.getBackGroundColor());
		m_pmm->addPlayer(HPlayerPositionType::Center, p, -1);
		hbox->addWidget(p);
	}
	else if (info.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		HPlayer *pl = new HVIPlayer(-1, NULL, QDir(), info.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, info.getBackGroundColor());
		m_pmm->addPlayer(HPlayerPositionType::Left, pl, -1);
		hbox->addWidget(pl);
		HPlayer *pr = new HVIPlayer(-1, NULL, QDir(), info.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, info.getBackGroundColor());
		m_pmm->addPlayer(HPlayerPositionType::Right, pr, -1);
		hbox->addWidget(pr);
	}
	pwidget->setLayout(hbox);

	QPushButton* playButton = new QPushButton(tr("Play"));
	QPushButton* clearButton = new QPushButton(tr("Clear"));

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	buttonBox->addButton(playButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(clearButton, QDialogButtonBox::ActionRole);

    connect(buttonBox, SIGNAL(rejected()), &dlg, SLOT(reject()));
    connect(buttonBox, SIGNAL(accepted()), &dlg, SLOT(accept()));
    connect(playButton, SIGNAL(clicked()), this, SLOT(playClicked()));
    connect(clearButton, SIGNAL(clicked()), this, SLOT(clearClicked()));
    connect(&dlg, SIGNAL(finished(int)), this, SLOT(playerFinished(int)));

    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->addWidget(pwidget);
    vbox->addWidget(buttonBox);
    dlg.setLayout(vbox);
    dlg.exec();

    if (m_pmm)
    {
    	delete m_pmm;
    	m_pmm = NULL;
    }

}

void SDIDialog::playClicked()
{
	m_pmm->stim(1);
}

void SDIDialog::clearClicked()
{
	m_pmm->clear();
}

void SDIDialog::playerFinished(int unused)
{
	Q_UNUSED(unused);
	delete m_pmm;
	m_pmm = NULL;
}
