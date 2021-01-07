/*
 * HStimulusPreviewWidget.cpp
 *
 *  Created on: Jul 8, 2015
 *      Author: Oakeslab
 */


#include "HStimulusPreviewWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStyle>

GUILib::HStimulusPreviewWidget::HStimulusPreviewWidget(const Habit::StimulusDisplayInfo& info, QWidget *parent)
: QWidget(parent)
, m_sdinfo(info)
, m_pCurrentStimulusWidget(NULL)
, m_bSingleStimulus(false)
, m_bListStimulus(false)
, m_bPlaying(false)
{
	//qDebug() << "HStimulusPreviewWidget::HStimulusPreviewWidget(" << (parent ? "parent" : "NULL") << ")";
	initialize(info);
}

GUILib::HStimulusPreviewWidget::HStimulusPreviewWidget(const Habit::StimulusDisplayInfo& info, unsigned int key, QWidget *parent)
: QWidget(parent)
, m_sdinfo(info)
, m_bSingleStimulus(true)
, m_bListStimulus(false)
, m_currentStimKey(key)
, m_bPlaying(false)
{
	initialize(info);
}

void GUILib::HStimulusPreviewWidget::initialize(const Habit::StimulusDisplayInfo& info)
{
	// set up label for stim name and buttons for advancing stim through orders.
	m_pbNext = new QPushButton(">");
	m_pbPrev = new QPushButton("<");
	m_pbPause = new QPushButton();
	m_pbPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
	m_pbPlay = new QPushButton();
	m_pbPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
	m_pbRewind = new QPushButton();
	m_pbRewind->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
	m_labelStimName = new QLabel("Stimulus: ");
	QHBoxLayout *hlb = new QHBoxLayout;
	hlb->addWidget(m_labelStimName, 1, Qt::AlignCenter);
	hlb->addStretch(1);
	hlb->addWidget(m_pbPrev);
	hlb->addWidget(m_pbRewind);
	hlb->addWidget(m_pbPlay);
	hlb->addWidget(m_pbPause);
	hlb->addWidget(m_pbNext);

	// connect up all the buttons
	connect(m_pbPrev, SIGNAL(clicked()), this, SLOT(prevClicked()));
	connect(m_pbNext, SIGNAL(clicked()), this, SLOT(nextClicked()));
	connect(m_pbPause, SIGNAL(clicked()), this, SLOT(pauseClicked()));
	connect(m_pbPlay, SIGNAL(clicked()), this, SLOT(playClicked()));
	connect(m_pbRewind, SIGNAL(clicked()), this, SLOT(rewindClicked()));

	// Now do the layout for the entire thing
    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->addWidget(m_pCurrentStimulusWidget = createStimulusWidget(info));
    vbox->addLayout(hlb);
    setLayout(vbox);

    // This turns on/off nav buttons buttons
    updateNavigation();

}

GUILib::HStimulusPreviewWidget::~HStimulusPreviewWidget()
{
}


QWidget* GUILib::HStimulusPreviewWidget::createStimulusWidget(const Habit::StimulusDisplayInfo& info)
{
	QWidget *pWidget = new QWidget;
	QHBoxLayout *hbox = new QHBoxLayout;

	if (info.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		HStimulusWidget *w = new HStimulusWidget(info, 320, 240);
		hbox->addWidget(w);
		m_vecWidgets.append(w);
		pWidget->setLayout(hbox);
	}
	else if (info.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		HStimulusWidget *w = new HStimulusWidget(info, 320, 240);
		hbox->addWidget(w);
		m_vecWidgets.append(w);
		w = new HStimulusWidget(info, 320, 240);
		hbox->addWidget(w);
		m_vecWidgets.append(w);
		pWidget->setLayout(hbox);
	}
	else if (info.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutTriple)
	{
		HStimulusWidget *w = new HStimulusWidget(info, 320, 240);
		hbox->addWidget(w);
		m_vecWidgets.append(w);
		w = new HStimulusWidget(info, 320, 240);
		hbox->addWidget(w);
		m_vecWidgets.append(w);
		w = new HStimulusWidget(info, 320, 240);
		hbox->addWidget(w);
		m_vecWidgets.append(w);
		pWidget->setLayout(hbox);
	}
	return pWidget;
}



QVector<HStimulusWidget *> GUILib::HStimulusPreviewWidget::getStimulusWidgets()
{
	return m_vecWidgets;
}


void GUILib::HStimulusPreviewWidget::prevClicked()
{
	m_idListCurrent--;
	updateNavigation();

	emit preroll(m_idList[m_idListCurrent]);
	emit stim(m_idList[m_idListCurrent]);
}

void GUILib::HStimulusPreviewWidget::nextClicked()
{
	m_idListCurrent++;
	updateNavigation();
	emit preroll(m_idList[m_idListCurrent]);
	emit stim(m_idList[m_idListCurrent]);
}

void GUILib::HStimulusPreviewWidget::pauseClicked()
{
	if (m_bSingleStimulus)
	{
		emit pause(m_currentStimKey);
		m_bPlaying = false;
	}
	else if (m_bListStimulus)
	{
		emit pause(m_idList[m_idListCurrent]);
		m_bPlaying = false;
	}
	updateNavigation();
}

void GUILib::HStimulusPreviewWidget::stopClicked()
{
	m_bSingleStimulus = true;
	m_currentStimKey = 0;
	m_bListStimulus = false;
	emit background();
	m_bPlaying = true;
	updateNavigation();
	emit stopped();
}


void GUILib::HStimulusPreviewWidget::playClicked()
{
	if (m_bSingleStimulus)
	{
		emit stim(m_currentStimKey);
		m_bPlaying = true;
	}
	else if (m_bListStimulus)
	{
		emit stim(m_idList[m_idListCurrent]);
		m_bPlaying = true;
	}
	updateNavigation();
}

void GUILib::HStimulusPreviewWidget::rewindClicked()
{
	if (m_bSingleStimulus)
	{
		emit rewind(m_currentStimKey);
		m_bPlaying = true;
	}
	else if (m_bListStimulus)
	{
		emit rewind(m_idList[m_idListCurrent]);
		m_bPlaying = true;
	}
	updateNavigation();
}

// update nav buttons
void GUILib::HStimulusPreviewWidget::updateNavigation()
{
	if (m_bSingleStimulus)
	{
		// QString stimName = m_hgmm.getStimulusSettings(m_currentStimKey).getName();
		QString stimName("TODO");
		m_labelStimName->setText(QString("Stimulus: %1").arg(stimName));
		m_pbPrev->setEnabled(false);
		m_pbNext->setEnabled(false);
		if (m_bPlaying)
		{
			m_pbPlay->setEnabled(false);
			m_pbPause->setEnabled(true);
			m_pbRewind->setEnabled(true);
			//m_pbStop->setEnabled(true);
		}
		else
		{
			m_pbPlay->setEnabled(true);
			m_pbPause->setEnabled(false);
			m_pbRewind->setEnabled(false);
			//m_pbStop->setEnabled(false);
		}
	}
	else if (m_bListStimulus)
	{
		//QString stimName = m_hgmm.getStimulusSettings(m_idList[m_idListCurrent]).getName();
		QString stimName("TODO");
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
			m_pbPause->setEnabled(true);
			m_pbRewind->setEnabled(true);
		}
		else
		{
			m_pbPlay->setEnabled(true);
			m_pbPause->setEnabled(false);
			m_pbRewind->setEnabled(false);
		}
	}
	else
	{
		m_pbPrev->setEnabled(false);
		m_pbNext->setEnabled(false);
		m_pbPlay->setEnabled(false);
		m_pbPause->setEnabled(false);
		m_pbRewind->setEnabled(false);
		m_labelStimName->setText(QString("None"));
	}
}

void GUILib::HStimulusPreviewWidget::preview(unsigned int key, bool bPlayNow)
{
	m_bSingleStimulus = true;
	m_currentStimKey = key;
	m_bListStimulus = false;
	//emit preroll(key);
	if (bPlayNow)
	{
		emit stim(key);
		m_bPlaying = true;
	}
	else
	{
		m_bPlaying = false;
	}
	updateNavigation();
}


void GUILib::HStimulusPreviewWidget::preview(const QList<unsigned int>& list, bool bPlayNow)
{
	m_bSingleStimulus = false;
	m_bListStimulus = true;
	m_idList = list;
	m_idListCurrent = 0;
	emit preroll(m_idList[m_idListCurrent]);
	if (bPlayNow)
	{
		// this call just sets pipeline state to PLAYING
		emit stim(m_idList[m_idListCurrent]);
		m_bPlaying = true;
	}
	else
	{
		m_bPlaying = false;
	}
	updateNavigation();
}

void GUILib::HStimulusPreviewWidget::clear()
{
	emit background();
	m_bPlaying = true;
	m_bSingleStimulus = true;
	m_bListStimulus = false;
	m_currentStimKey = 0;
}

void GUILib::HStimulusPreviewWidget::hideEvent(QHideEvent *)
{
//	//qDebug() << "hide event";
//	// current state of the widget is held in the vars
//	// m_bSingleStimulus, m_currentStimKey - when a single stim is playing
//	// m_bListStimulus, m_idList, m_idListCurrent - when an order is playing
//	m_hgmm.defaultStim();	// this forces cleanup on the stim that's playing
//	m_hgmm.setWidgets(NULL, NULL, NULL);
}

void GUILib::HStimulusPreviewWidget::showEvent(QShowEvent *)
{
	// TODO should not be necessary any more once HGMM instance work done.
//	//qDebug() << "show event";
//	m_hgmm.setWidgets(m_w0, m_w1, m_w2);
//	if (m_bSingleStimulus)
//	{
//		if (m_bPlaying)
//		{
//			m_hgmm.stim(m_currentStimKey);
//		}
//		else
//		{
//			m_hgmm.pause(m_currentStimKey);
//		}
//	}
//	else if (m_bListStimulus)
//	{
//		if (m_bPlaying)
//		{
//			m_hgmm.stim(m_idList[m_idListCurrent]);
//		}
//		else
//		{
//			m_hgmm.pause(m_idList[m_idListCurrent]);
//		}
//	}
//	else
//		m_hgmm.background();
//	updateNavigation();
}

void GUILib::HStimulusPreviewWidget::setStimulusLayoutType(const HStimulusLayoutType&)
{
	//qDebug() << "HStimulusPreviewWidget::setStimulusLayoutType(" << type.name() << ")";
//	m_pmm->setLayoutType(type);
//	if (type == HStimulusLayoutType::HStimulusLayoutSingle)
//		m_pmm->stack()->setCurrentIndex(0);
//	else if (type == HStimulusLayoutType::HStimulusLayoutLeftRight)
//		m_pmm->stack()->setCurrentIndex(1);
}

