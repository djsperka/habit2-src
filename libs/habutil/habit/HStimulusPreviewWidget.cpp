/*
 * HStimulusPreviewWidget.cpp
 *
 *  Created on: Jul 8, 2015
 *      Author: Oakeslab
 */


#include "HStimulusPreviewWidget.h"
#include "HMediaManagerUtil.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

const int f_previewContext = 0;

GUILib::HStimulusPreviewWidget::HStimulusPreviewWidget(const Habit::StimulusDisplayInfo& info, QWidget *parent)
: QWidget(parent)
, m_bSingleStimulus(false)
, m_bListStimulus(false)
, m_bPlaying(false)
{
	qDebug() << "HStimulusPreviewWidget::HStimulusPreviewWidget(" << (parent ? "parent" : "NULL") << ")";
	m_pmm = createMediaManager(info, 320, 240);

	// make an hbox to hold the widgets
	QHBoxLayout *hbox = new QHBoxLayout;

	if (info.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		hbox->addStretch(1);
		//hbox->addWidget(m_pmm->getHStimulusWidget(HPlayerPositionType::Center));
		hbox->addWidget(m_pmm->createStimulusWidget());
		hbox->addStretch(1);
	}
	else if (info.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		hbox->addStretch(1);
		hbox->addWidget(m_pmm->createStimulusWidget());
		hbox->addStretch(1);
//		hbox->addStretch(2);
//		hbox->addWidget(m_pmm->getHStimulusWidget(HPlayerPositionType::Left));
//		hbox->addStretch(1);
//		hbox->addWidget(m_pmm->getHStimulusWidget(HPlayerPositionType::Right));
//		hbox->addStretch(2);
	}

	// set up label for stim name and buttons for advancing stim through orders.
	m_pbNext = new QPushButton(">");
	m_pbPrev = new QPushButton("<");
	m_pbStop = new QPushButton("Stop");
	m_pbPlay = new QPushButton("Play");
	m_pbRewind = new QPushButton("Rewind");
	m_labelStimName = new QLabel("Stimulus: ");
	QHBoxLayout *hlb = new QHBoxLayout;
	hlb->addWidget(m_labelStimName, 1, Qt::AlignCenter);
	hlb->addStretch(1);
	hlb->addWidget(m_pbPrev);
	hlb->addWidget(m_pbRewind);
	hlb->addWidget(m_pbPlay);
	hlb->addWidget(m_pbStop);
	hlb->addWidget(m_pbNext);

	connect(m_pbPrev, SIGNAL(clicked()), this, SLOT(prevClicked()));
	connect(m_pbNext, SIGNAL(clicked()), this, SLOT(nextClicked()));
	connect(m_pbStop, SIGNAL(clicked()), this, SLOT(stopClicked()));
	connect(m_pbPlay, SIGNAL(clicked()), this, SLOT(playClicked()));
	connect(m_pbRewind, SIGNAL(clicked()), this, SLOT(rewindClicked()));

	// Now do the layout for the entire thing
    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->addLayout(hbox);
    //vbox->addWidget(m_pmm->stack());
    vbox->addLayout(hlb);
    setLayout(vbox);

    // This turns on/off nav buttons buttons
    updateNavigation();

}

GUILib::HStimulusPreviewWidget::~HStimulusPreviewWidget()
{
	qDebug() << "HStimulusPreviewWidget::~HStimulusPreviewWidget()";
	m_pmm->stop();
	delete m_pmm;
}

void GUILib::HStimulusPreviewWidget::prevClicked()
{
	m_idListCurrent--;
	updateNavigation();
	m_pmm->preroll(m_idList[m_idListCurrent]);
	m_pmm->stim(m_idList[m_idListCurrent]);
}

void GUILib::HStimulusPreviewWidget::nextClicked()
{
	m_idListCurrent++;
	updateNavigation();
	m_pmm->preroll(m_idList[m_idListCurrent]);
	m_pmm->stim(m_idList[m_idListCurrent]);
}

void GUILib::HStimulusPreviewWidget::stopClicked()
{
	if (m_bSingleStimulus)
	{
		m_pmm->pause(m_iCurrentStimulusIndex);
		m_bPlaying = false;
	}
	else if (m_bListStimulus)
	{
		m_pmm->pause(m_idList[m_idListCurrent]);
		m_bPlaying = false;
	}
	updateNavigation();
}

void GUILib::HStimulusPreviewWidget::playClicked()
{
	if (m_bSingleStimulus)
	{
		m_pmm->stim(m_iCurrentStimulusIndex);
		m_bPlaying = true;
	}
	else if (m_bListStimulus)
	{
		m_pmm->stim(m_idList[m_idListCurrent]);
		m_bPlaying = true;
	}
	updateNavigation();
}

void GUILib::HStimulusPreviewWidget::rewindClicked()
{
	if (m_bSingleStimulus)
	{
		m_pmm->rewind(m_iCurrentStimulusIndex);
		m_bPlaying = true;
	}
	else if (m_bListStimulus)
	{
		m_pmm->rewind(m_idList[m_idListCurrent]);
		m_bPlaying = true;
	}
	updateNavigation();
}

void GUILib::HStimulusPreviewWidget::preview(const Habit::StimulusSettings& stimulus, bool bPlay)
{
	// cleanup any stimuli in pmm, return to background
	clear();

	// add new stimulus to media manager
	m_bSingleStimulus = true;
	m_bListStimulus = false;
	m_iCurrentStimulusIndex = m_pmm->addStimulus(stimulus, f_previewContext);
	m_pmm->preroll(m_iCurrentStimulusIndex);
	if (bPlay)
	{
		m_pmm->stim(m_iCurrentStimulusIndex);
		m_bPlaying = true;
	}
	updateNavigation();
}

// update nav buttons
void GUILib::HStimulusPreviewWidget::updateNavigation()
{
	if (m_bSingleStimulus)
	{
		m_pbPrev->setEnabled(false);
		m_pbNext->setEnabled(false);
		if (m_bPlaying)
		{
			m_pbPlay->setEnabled(false);
			m_pbStop->setEnabled(true);
			m_pbRewind->setEnabled(true);
		}
		else
		{
			m_pbPlay->setEnabled(true);
			m_pbStop->setEnabled(false);
			m_pbRewind->setEnabled(false);
		}
	}
	else if (m_bListStimulus)
	{
		QString stimName = m_pmm->getStimulusSettings(m_idList[m_idListCurrent]).getName();
		m_labelStimName->setText(QString("Stimulus (%1/%2): %3").arg(m_idListCurrent+1).arg(m_idList.count()).arg(stimName));
		if (m_idListCurrent > 0)
			m_pbPrev->setEnabled(true);
		else
			m_pbPrev->setEnabled(false);
		if (m_idListCurrent < (m_idList.count()-1))
			m_pbNext->setEnabled(true);
		else
			m_pbNext->setEnabled(false);
		if (m_bPlaying)
		{
			m_pbPlay->setEnabled(false);
			m_pbStop->setEnabled(true);
			m_pbRewind->setEnabled(true);
		}
		else
		{
			m_pbPlay->setEnabled(true);
			m_pbStop->setEnabled(false);
			m_pbRewind->setEnabled(false);
		}
	}
	else
	{
		m_pbPrev->setEnabled(false);
		m_pbNext->setEnabled(false);
		m_pbPlay->setEnabled(false);
		m_pbStop->setEnabled(false);
		m_pbRewind->setEnabled(false);
		m_labelStimName->setText(QString("None"));
	}
}


void GUILib::HStimulusPreviewWidget::preview(const Habit::HStimulusSettingsList& stimuli, QList< QPair<int, QString> > list)
{
	unsigned int stimid;
	QPair<int, QString> p;
	clear();
	m_idList.clear();
	foreach(p, list)
	{
		stimid = m_pmm->addStimulus(stimuli.at(p.first), f_previewContext);
		m_idList.append(stimid);
		qDebug() << "HStimulusPreviewWidget::preview: add stim index " << p.first << " label " << p.second << " stimid " << stimid;
	}
	m_idListCurrent = 0;
	m_bSingleStimulus = false;
	m_bListStimulus = true;
	updateNavigation();
	m_pmm->preroll(m_idList[m_idListCurrent]);
	m_pmm->stim(m_idList[m_idListCurrent]);
}

void GUILib::HStimulusPreviewWidget::clear()
{
//	m_pmm->clear();
	m_pmm->background();
}

void GUILib::HStimulusPreviewWidget::hideEvent(QHideEvent *event)
{
	qDebug() << "hide event";
}

void GUILib::HStimulusPreviewWidget::showEvent(QShowEvent *event)
{
	qDebug() << "show event";
}

void GUILib::HStimulusPreviewWidget::setStimulusLayoutType(const HStimulusLayoutType& type)
{
	qDebug() << "HStimulusPreviewWidget::setStimulusLayoutType(" << type.name() << ")";
//	m_pmm->setLayoutType(type);
//	if (type == HStimulusLayoutType::HStimulusLayoutSingle)
//		m_pmm->stack()->setCurrentIndex(0);
//	else if (type == HStimulusLayoutType::HStimulusLayoutLeftRight)
//		m_pmm->stack()->setCurrentIndex(1);
}

