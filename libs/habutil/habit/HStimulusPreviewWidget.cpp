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

GUILib::HStimulusPreviewWidget::HStimulusPreviewWidget(const Habit::StimulusDisplayInfo& info, const QDir& dirStimRoot, QWidget *parent)
: QWidget(parent)
{

	m_pmm = createMediaManager(info);
	m_pmm->addBackground(info.getBackGroundColor());

	// make an hbox to hold the widgets
	QHBoxLayout *hbox = new QHBoxLayout;

	if (info.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		hbox->addStretch(1);
		hbox->addWidget(m_pmm->getHStimulusWidget(HPlayerPositionType::Center));
		hbox->addStretch(1);
	}
	else if (info.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		hbox->addStretch(2);
		hbox->addWidget(m_pmm->getHStimulusWidget(HPlayerPositionType::Left));
		hbox->addStretch(1);
		hbox->addWidget(m_pmm->getHStimulusWidget(HPlayerPositionType::Right));
		hbox->addStretch(2);
	}

	// set up label for stim name and buttons for advancing stim through orders.
	m_pbNext = new QPushButton(">");
	m_pbPrev = new QPushButton("<");
	m_pbStop = new QPushButton("Stop");
	m_labelStimName = new QLabel("Stimulus: ");
	QHBoxLayout *hlb = new QHBoxLayout;
	hlb->addWidget(m_labelStimName, 1, Qt::AlignCenter);
	hlb->addStretch(1);
	hlb->addWidget(m_pbPrev);
	hlb->addWidget(m_pbStop);
	hlb->addWidget(m_pbNext);

	connect(m_pbPrev, SIGNAL(clicked()), this, SLOT(prevClicked()));
	connect(m_pbNext, SIGNAL(clicked()), this, SLOT(nextClicked()));
	connect(m_pbStop, SIGNAL(clicked()), this, SLOT(stopClicked()));

	// Now do the layout for the entire thing
    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->addLayout(hbox);
    //vbox->addWidget(m_pmm->stack());
    vbox->addLayout(hlb);
    setLayout(vbox);

    // This turns off the arow buttons
    updateNavigation(QString(""));

}

GUILib::HStimulusPreviewWidget::~HStimulusPreviewWidget()
{
	m_pmm->clear();
	delete m_pmm;
}

void GUILib::HStimulusPreviewWidget::prevClicked()
{
	m_idListCurrent--;
	updateNavigation();
	m_pmm->stim(m_idList[m_idListCurrent]);
}

void GUILib::HStimulusPreviewWidget::nextClicked()
{
	m_idListCurrent++;
	updateNavigation();
	m_pmm->stim(m_idList[m_idListCurrent]);
}

void GUILib::HStimulusPreviewWidget::stopClicked()
{
	m_pmm->clear();
}

void GUILib::HStimulusPreviewWidget::preview(const Habit::StimulusSettings& stimulus)
{
	unsigned int stimid;
	m_pmm->clear();
	stimid = m_pmm->addStimulus(stimulus, f_previewContext);
	updateNavigation(stimulus.getName());
	m_pmm->stim(stimid);
}

void GUILib::HStimulusPreviewWidget::updateNavigation(QString stimName)
{
	m_pbPrev->setEnabled(false);
	m_pbNext->setEnabled(false);
	m_labelStimName->setText(QString("Stimulus: %1").arg(stimName));
}

void GUILib::HStimulusPreviewWidget::updateNavigation()
{
	if (m_idListCurrent > 0)
		m_pbPrev->setEnabled(true);
	else
		m_pbPrev->setEnabled(false);
	if (m_idListCurrent < (m_idList.count()-1))
		m_pbNext->setEnabled(true);
	else
		m_pbNext->setEnabled(false);

	QString stimName = m_pmm->getStimulusSettings(m_idList[m_idListCurrent]).getName();
	m_labelStimName->setText(QString("Stimulus (%1/%2): %3").arg(m_idListCurrent+1).arg(m_idList.count()).arg(stimName));
}


void GUILib::HStimulusPreviewWidget::preview(const Habit::HStimulusSettingsList& stimuli, QList< QPair<int, QString> > list)
{
	unsigned int stimid;
	QPair<int, QString> p;
	m_pmm->clear();
	m_idList.clear();
	foreach(p, list)
	{
		stimid = m_pmm->addStimulus(stimuli.at(p.first), f_previewContext);
		m_idList.append(stimid);
		//qDebug() << "add stim index " << p.first << " label " << p.second << " stimid " << stimid;
	}
	m_idListCurrent = 0;
	updateNavigation();
	m_pmm->stim(m_idList[m_idListCurrent]);
}

void GUILib::HStimulusPreviewWidget::clear()
{
	m_pmm->clear();
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

