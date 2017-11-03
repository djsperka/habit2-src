/*
 * Q5PlayerTestDialog.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: dan
 */

#include "HG2Dialog.h"

#include <QDialogButtonBox>
#include <QDesktopWidget>
#include <QtDebug>

#include "stimulisettings.h"
#include "stimulusdisplayinfo.h"

#define MAX_INFO 5
Habit::StimulusInfo f_vinfo[MAX_INFO];
Habit::StimulusInfo f_ainfo[MAX_INFO];
Habit::StimuliSettings f_stimuli;

HG2Dialog::HG2Dialog(const QDir& dirStimRoot, int screen, bool bUseToggle, QWidget *parent)
: QDialog(parent)
, m_pb(NULL)
, m_pVideoWidgetLeft(NULL)
, m_pVideoWidgetRight(NULL)
, m_pVideoWidgetCenter(NULL)
, m_iCurrent(-1)
{
	QDialogButtonBox *buttonBox;
	QVBoxLayout *vbox;
	buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel, Qt::Horizontal, parent);
	m_pb = new QPushButton("Play(0)");
	vbox = new QVBoxLayout;
	buttonBox->addButton(m_pb, QDialogButtonBox::ActionRole);
	connect(m_pb, SIGNAL(clicked()), this, SLOT(playClicked()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(accept()));

	if (screen < 1)
	{
		vbox->addWidget(new QLabel("HELLO"));
	}
	else if (screen == 1)
	{
		Habit::StimulusDisplayInfo sdi;
		sdi.setBackGroundColor(QColor(Qt::blue));
		sdi.setDisplayType(HDisplayType::HDisplayTypeOriginalSize);
		sdi.setMaintainOriginalAspectRatio(true);
		sdi.setPresentationStyle(HPresentationStyle::HPresentationStyleMonitorDefined);
		sdi.setStimulusLayoutType(HStimulusLayoutType::HStimulusLayoutSingle);
		sdi.setUseISS(true);
		vbox->addLayout(initSingleScreen(sdi, dirStimRoot));
	}
	else
	{
		Habit::StimulusDisplayInfo sdi;
		sdi.setBackGroundColor(QColor(Qt::blue));
		sdi.setDisplayType(HDisplayType::HDisplayTypeOriginalSize);
		sdi.setMaintainOriginalAspectRatio(true);
		sdi.setPresentationStyle(HPresentationStyle::HPresentationStyleMonitorDefined);
		sdi.setStimulusLayoutType(HStimulusLayoutType::HStimulusLayoutLeftRight); // not used?
		sdi.setUseISS(true);
		vbox->addLayout(initLRScreen(sdi, dirStimRoot));
	}
	vbox->addWidget(buttonBox);
	setLayout(vbox);


	//connect(m_paudioHGstPlayer, SIGNAL(started(int, const QString&)), this, SLOT(started(int, const QString& )));


}

HG2Dialog::~HG2Dialog()
{
	//delete m_paudioHGstPlayer;
	//m_pvideoHGstPlayer->stop();
	//delete m_pvideoHGstPlayer;
}

void HG2Dialog::stimStarted(int key)
{
	qDebug() << "HG2Dialog::stimStarted " << key;
}

void HG2Dialog::agStarted()
{
	qDebug() << "HG2Dialog::agStarted";
}

QHBoxLayout *HG2Dialog::initSingleScreen(const Habit::StimulusDisplayInfo& sdi, const QDir& dirStimRoot)
{
	QHBoxLayout *hbox = new QHBoxLayout;
	m_pVideoWidgetCenter = new HStimulusWidget(sdi, 1024, 768);
	//m_pVideoWidgetCenter->setMinimumSize(320, 240);
	hbox->addWidget(m_pVideoWidgetCenter);

	m_pmm = new HGMM(m_pVideoWidgetCenter, dirStimRoot, true, sdi.getBackGroundColor());
	//connect(m_pmm, SIGNAL(mmReady()), this, SLOT(mmReady()));
	//connect(m_pmm, SIGNAL(mmFail()), this, SLOT(mmFail()));
	connect(m_pmm, SIGNAL(agStarted()), this, SLOT(agStarted()));
	connect(m_pmm, SIGNAL(stimStarted(int)), this, SLOT(stimStarted(int)));
	connect(m_pmm, SIGNAL(stimulusChanged()), m_pVideoWidgetCenter->getHVideoWidget(), SLOT(stimulusChanged()));

	// create background
//	m_pmm->addBackground(QColor(Qt::green));

	// pic
	{
		Habit::StimulusSettings s2;
		s2.setName("s2-ocean-nosound");
		Habit::StimulusInfo si2(QString("oceanz"), QString("images/posters/ocean.jpg"));
		s2.setCenterStimulusInfo(si2);
		f_stimuli.addStimulus(s2);
	}

	// movie
	{
		Habit::StimulusSettings s3;
		s3.setName("s3-taxi-nosound");
		Habit::StimulusInfo si3(QString("taxi"), QString("mov/taxi-480p.mov"), true);
		s3.setCenterStimulusInfo(si3);
		f_stimuli.addStimulus(s3);
	}

	{
		Habit::StimulusSettings s3;
		s3.setName("s4-avi-nosound");
		Habit::StimulusInfo si3(QString("taxi"), QString("avi/avi1.avi"), true);
		s3.setCenterStimulusInfo(si3);
		f_stimuli.addStimulus(s3);
	}

	{
		Habit::StimulusSettings s4;
		s4.setName("s5-excavator-nosound");
		Habit::StimulusInfo si4(QString("excavator"), QString("images/tools/excavator.png"));
		s4.setCenterStimulusInfo(si4);
		f_stimuli.addStimulus(s4);
	}

	{
		// color + sound no loop
		Habit::StimulusSettings s6;
		s6.setName("s6-tire-whistle");
		Habit::StimulusInfo si6(QString("tire"), QString("images/tools/tire.png"));
		s6.setCenterStimulusInfo(si6);
		Habit::StimulusInfo sis6("wav/slide_whistle_x.wav", true, 50);
		s6.setIndependentSoundInfo(sis6);
		f_stimuli.addStimulus(s6);
	}

	{
		// color + sound no loop
		Habit::StimulusSettings s5;
		s5.setName("s7-green-bowling");
		Habit::StimulusInfo si5(Qt::green);
		s5.setCenterStimulusInfo(si5);
		Habit::StimulusInfo sis5("wav/bowling.wav", true, 50);
		s5.setIndependentSoundInfo(sis5);
		f_stimuli.addStimulus(s5);
	}

	m_pmm->addStimuli(f_stimuli, 1);

	//m_pmm->getReady(5000);

	m_pmm->preroll(0);
	m_pmm->preroll(1);
	//m_pmm->stim(0);
	m_iCurrent = 0;

	return hbox;
}


QHBoxLayout *HG2Dialog::initLRScreen(const Habit::StimulusDisplayInfo& sdi, const QDir& dirStimRoot)
{
	QHBoxLayout *hbox = new QHBoxLayout;
	m_pVideoWidgetLeft = new HStimulusWidget(sdi, 320, 240);
	m_pVideoWidgetLeft->setMinimumSize(320, 240);
	m_pVideoWidgetRight = new HStimulusWidget(sdi, 320, 240);
	m_pVideoWidgetRight->setMinimumSize(320, 240);
	hbox->addWidget(m_pVideoWidgetLeft);
	hbox->addWidget(m_pVideoWidgetRight);

	m_pmm = new HGMM(m_pVideoWidgetLeft, m_pVideoWidgetRight, dirStimRoot);
	//connect(m_pmm, SIGNAL(mmReady()), this, SLOT(mmReady()));
	//connect(m_pmm, SIGNAL(mmFail()), this, SLOT(mmFail()));
	connect(m_pmm, SIGNAL(agStarted()), this, SLOT(agStarted()));
	connect(m_pmm, SIGNAL(stimStarted(int)), this, SLOT(stimStarted(int)));
	connect(m_pmm, SIGNAL(stimulusChanged()), m_pVideoWidgetLeft->getHVideoWidget(), SLOT(stimulusChanged()));
	connect(m_pmm, SIGNAL(stimulusChanged()), m_pVideoWidgetRight->getHVideoWidget(), SLOT(stimulusChanged()));

	// create background
	m_pmm->addBackground(QColor(Qt::green));

	// pic
	Habit::StimulusSettings s1;
	s1.setName("s1");
	Habit::StimulusInfo si1L(QString("ocean"), QString("images/posters/ocean.jpg"));
	s1.setLeftStimulusInfo(si1L);
	Habit::StimulusInfo si1R(QString("owl"), QString("images/posters/owl.jpg"));
	s1.setRightStimulusInfo(si1R);

	f_stimuli.addStimulus(s1);

	// movie
	Habit::StimulusSettings s2;
	s2.setName("s2");
	Habit::StimulusInfo si2L(QString("earthsun"), QString("mov/earthsun.mov"), false);
	s2.setLeftStimulusInfo(si2L);
	Habit::StimulusInfo si2R(QString("taxi"), QString("mov/taxi-480p.mov"), true);
	s2.setRightStimulusInfo(si2R);
	f_stimuli.addStimulus(s2);

	Habit::StimulusSettings s3;
	s3.setName("s3");
	Habit::StimulusInfo si3L(QString("small"), QString("mov/small.mp4"), true);
	s3.setLeftStimulusInfo(si3L);
	Habit::StimulusInfo si3R(QString("tire"), QString("images/tools/tire.png"));
	s3.setRightStimulusInfo(si3R);
	f_stimuli.addStimulus(s3);

	Habit::StimulusSettings s4;
	s4.setName("s4");
	Habit::StimulusInfo si4L(QString("hydrant"), QString("images/tools/hydrant.png"));
	s4.setLeftStimulusInfo(si4L);
	Habit::StimulusInfo si4R(QString("excavator"), QString("images/tools/excavator.png"));
	s4.setRightStimulusInfo(si4R);
	f_stimuli.addStimulus(s4);

	m_pmm->addStimuli(f_stimuli, 1);

	m_pmm->preroll(0);
	m_pmm->preroll(1);
	m_pmm->stim(0);
	m_iCurrent = 0;

	//m_pmm->getReady(5000);
	return hbox;
}


#if 0
void HG2Dialog::mediaChanged(libvlc_media_t *media)
{
	qInfo() << "media changed";
	// what can we find out about the media?

}
#endif

void HG2Dialog::started(int screenid, const QString& filename)
{
	qInfo() << "started " << screenid << ":" << filename;
}

void HG2Dialog::playClicked()
{
	unsigned int iNext;
	unsigned int iPreroll;

	// currently playing stimulus is index 'm_iCurrent'
	// iNext is the stim we will play next/now
	iNext = m_iCurrent+1;
	if (iNext == m_pmm->nStimuli())
		iNext = 0;

	// iNext should be prerolled already, iPreroll should be prerolled now
	iPreroll = iNext + 1;
	if (iPreroll == m_pmm->nStimuli())
		iPreroll = 0;

	qDebug() << " preroll stim " << iPreroll;
	m_pmm->preroll(iPreroll);

	qDebug() << "play stim " << iNext;
	m_pmm->stim(iNext);

	qDebug() << "ready stim " << m_iCurrent;
	m_pmm->ready(m_iCurrent);

	// update currently playing stim
	m_iCurrent = iNext;

	// update button text
	m_pb->setText(QString("play(%1)").arg(iPreroll));
}

void HG2Dialog::mmReady()
{
	qDebug() << "HG2Dialog::mmReady";
}

void HG2Dialog::mmFail()
{
	qDebug() << "HG2Dialog::mmFail";
}
