/*
 * Q5PlayerTestDialog.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: dan
 */

#include <QDialogButtonBox>
#include <QDesktopWidget>
#include <QtDebug>

#include "stimulisettings.h"
#include "stimulusdisplayinfo.h"
#include "HG3Dialog.h"
#include <hgst/HPipeline.h>
#include <gst/gst.h>
#include <hgst/HStimPipeline.h>

#define MAX_INFO 5
Habit::StimulusInfo f_vinfo[MAX_INFO];
Habit::StimulusInfo f_ainfo[MAX_INFO];
Habit::StimuliSettings f_stimuli;


HG3Dialog::HG3Dialog(const QDir& dirStimRoot, int screen, const QString& flag, QWidget *parent)
: QDialog(parent)
, m_pVideoWidgetLeft(NULL)
, m_pVideoWidgetRight(NULL)
, m_pVideoWidgetCenter(NULL)
, m_iCurrent(-1)
{
	QDialogButtonBox *buttonBox;
	QVBoxLayout *vbox;
	buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel, Qt::Horizontal, parent);
	m_pbInitialize = new QPushButton("Init");
	m_pbCleanup = new QPushButton("Cleanup");
	m_pbPreroll = new QPushButton("Preroll");
	m_pbPause = new QPushButton("Pause");
	m_pbRewind = new QPushButton("Rewind");
	m_pbPlay = new QPushButton("PLay");
	m_pbDump = new QPushButton("Dump");
	vbox = new QVBoxLayout;
	//buttonBox->addButton(m_pbInitialize, QDialogButtonBox::ActionRole);
	buttonBox->addButton(m_pbCleanup, QDialogButtonBox::ActionRole);
	//buttonBox->addButton(m_pbPreroll, QDialogButtonBox::ActionRole);
	buttonBox->addButton(m_pbPause, QDialogButtonBox::ActionRole);
	//buttonBox->addButton(m_pbRewind, QDialogButtonBox::ActionRole);
	buttonBox->addButton(m_pbPlay, QDialogButtonBox::ActionRole);
	buttonBox->addButton(m_pbDump, QDialogButtonBox::ActionRole);
	//connect(m_pbInitialize, SIGNAL(clicked()), this, SLOT(initializeClicked()));
	//connect(m_pbPreroll, SIGNAL(clicked()), this, SLOT(prerollClicked()));
	//connect(m_pbRewind, SIGNAL(clicked()), this, SLOT(rewindClicked()));
	connect(m_pbPause, SIGNAL(clicked()), this, SLOT(pauseClicked()));
	connect(m_pbPlay, SIGNAL(clicked()), this, SLOT(playClicked()));
	connect(m_pbCleanup, SIGNAL(clicked()), this, SLOT(cleanupClicked()));
	connect(m_pbDump, SIGNAL(clicked()), this, SLOT(dumpClicked()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(rejected()));

	m_pbPreroll->setEnabled(false);
	m_pbRewind->setEnabled(false);
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
		vbox->addWidget(initSingleScreen(sdi, dirStimRoot, flag));
		//vbox->addLayout(initSingleScreen(sdi, dirStimRoot, flag));
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
		vbox->addWidget(initLRScreen(sdi, dirStimRoot, flag));
	}

	m_sbWhich = new QSpinBox(this);
	m_sbWhich->setMinimum(0);
	m_sbWhich->setMaximum(m_pmm->nStimuli()-1);

	vbox->addWidget(m_sbWhich);
	vbox->addWidget(buttonBox);
	setLayout(vbox);


	//connect(m_paudioHGstPlayer, SIGNAL(started(int, const QString&)), this, SLOT(started(int, const QString& )));


}

void HG3Dialog::rejected()
{
	m_pmm->reset();
	accept();
}

HG3Dialog::~HG3Dialog()
{
	//delete m_paudioHGstPlayer;
	//m_pvideoHGstPlayer->stop();
	//delete m_pvideoHGstPlayer;
	//delete m_pmm;
}

void HG3Dialog::stimStarted(int key)
{
	qDebug() << "HG3Dialog::stimStarted " << key;
}

void HG3Dialog::agStarted()
{
	qDebug() << "HG3Dialog::agStarted";
}

QWidget *HG3Dialog::initSingleScreen(const Habit::StimulusDisplayInfo& sdi, const QDir& dirStimRoot, const QString& flag)
{
//	QHBoxLayout *hbox = new QHBoxLayout;
	m_pVideoWidgetCenter = new HStimulusWidget(sdi, 1024, 768);
	m_pVideoWidgetCenter->setMinimumSize(320, 240);
//	hbox->addWidget(m_pVideoWidgetCenter);

	//m_pmm = new HGMM(m_pVideoWidgetCenter, dirStimRoot, true, sdi.getBackGroundColor(), HStimPipelineFactory);
	m_pmm = &HGMM::instance();
	m_pmm->reset(m_pVideoWidgetCenter, sdi, dirStimRoot);
	//connect(m_pmm, SIGNAL(mmReady()), this, SLOT(mmReady()));
	//connect(m_pmm, SIGNAL(mmFail()), this, SLOT(mmFail()));
	connect(m_pmm, SIGNAL(agStarted()), this, SLOT(agStarted()));
	connect(m_pmm, SIGNAL(stimStarted(int)), this, SLOT(stimStarted(int)));
	connect(m_pmm, SIGNAL(stimulusChanged()), m_pVideoWidgetCenter->getHVideoWidget(), SLOT(stimulusChanged()));

	// create background
//	m_pmm->addBackground(QColor(Qt::green));

	// pic
	if (flag.contains("all") || flag.contains("2"))
	{
		Habit::StimulusSettings s2;
		s2.setName("s2-ocean-nosound");
		Habit::StimulusInfo si2(QString("oceanz"), QString("images/posters/ocean.jpg"));
		s2.setCenterStimulusInfo(si2);
		f_stimuli.addStimulus(s2);
	}

	// movie - taxi hgas a sound stream
	if (flag.contains("all") || flag.contains("3"))
	{
		Habit::StimulusSettings s3;
		s3.setName("s3-taxi-nosound");
		Habit::StimulusInfo si3(QString("taxi"), QString("mov/taxi-480p.mov"), true);
		s3.setCenterStimulusInfo(si3);
		f_stimuli.addStimulus(s3);
	}

	if (flag.contains("all") || flag.contains("4"))
	{
		Habit::StimulusSettings s3;
		s3.setName("s4-drop-nosound");
		Habit::StimulusInfo si3(QString("drop"), QString("avi/toy_plane_liftoff.avi"), true);
		s3.setCenterStimulusInfo(si3);
		f_stimuli.addStimulus(s3);
	}

	if (flag.contains("all") || flag.contains("5"))
	{
		Habit::StimulusSettings s4;
		s4.setName("s5-excavator-nosound");
		Habit::StimulusInfo si4(QString("excavator"), QString("images/tools/excavator.png"));
		s4.setCenterStimulusInfo(si4);
		f_stimuli.addStimulus(s4);
	}

	if (flag.contains("all") || flag.contains("6"))
	{
		// color + sound no loop
		Habit::StimulusSettings s6;
		s6.setName("s6-tire-whistle");
		Habit::StimulusInfo si6(QString("tire"), QString("images/tools/tire.png"));
		s6.setCenterStimulusInfo(si6);
		Habit::StimulusInfo sis6("wav/slide_whistle_x.wav", false, 50);
		s6.setIndependentSoundInfo(sis6);
		f_stimuli.addStimulus(s6);
	}

	if (flag.contains("all") || flag.contains("7"))
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

	if (flag.contains("all") || flag.contains("8"))
	{
		// color + sound no loop
		Habit::StimulusSettings s8;
		s8.setName("s8-red-whistle-from-movie");
		Habit::StimulusInfo si8(Qt::red);
		s8.setCenterStimulusInfo(si8);
		Habit::StimulusInfo sis8("mov/Loop.mov", true, 50);
		s8.setIndependentSoundInfo(sis8);
		f_stimuli.addStimulus(s8);
	}

	if (flag.contains("all") || flag.contains("9"))
	{
		// taxi + sound BOTH loop
		Habit::StimulusSettings s9;
		s9.setName("s9-taxi-whistle-loop");
		Habit::StimulusInfo si9(QString("taxi"), QString("mov/taxi-480p.mov"), true);
		s9.setCenterStimulusInfo(si9);
		Habit::StimulusInfo sis9("wav/fanfare_x.wav", true, 30);
		s9.setIndependentSoundInfo(sis9);
		f_stimuli.addStimulus(s9);
	}

	if (flag.contains("all") || flag.contains("10"))
	{
		// taxi + sound BOTH loop
		Habit::StimulusSettings s9;
		s9.setName("s9-taxi-whistle-loop");
		Habit::StimulusInfo si9(QString("taxi"), QString("mov/taxi-480p.mov"), true);
		s9.setCenterStimulusInfo(si9);
		Habit::StimulusInfo sis9("wav/fanfare_x.wav", false, 30);
		s9.setIndependentSoundInfo(sis9);
		f_stimuli.addStimulus(s9);
	}

	if (flag.contains("all") || flag.contains("11"))
	{
		// taxi + sound BOTH loop
		Habit::StimulusSettings s10;
		s10.setName("s10-pdreams-sound-loop");
		Habit::StimulusInfo si10(QString("taxi"), QString("mp4/pdreams-clip.mp4"), true, 50);
		s10.setCenterStimulusInfo(si10);
		f_stimuli.addStimulus(s10);
	}

	m_pmm->addStimuli(f_stimuli, 1);

	//m_pmm->getReady(5000);

	//m_pmm->preroll(0);
	//m_pmm->preroll(1);
	//m_pmm->stim(0);
	m_iCurrent = 0;

	return m_pmm->createStimulusWidget();
}


QWidget *HG3Dialog::initLRScreen(const Habit::StimulusDisplayInfo& sdi, const QDir& dirStimRoot, const QString& flag)
{
	m_pVideoWidgetLeft = new HStimulusWidget(sdi, 1024, 768);
	m_pVideoWidgetLeft->setMinimumSize(320, 240);
	m_pVideoWidgetRight = new HStimulusWidget(sdi, 1024, 768);
	m_pVideoWidgetRight->setMinimumSize(320, 240);

	// NOTE: skipping ISS below.
	m_pmm = &HGMM::instance();
	m_pmm->reset(m_pVideoWidgetLeft, m_pVideoWidgetRight, sdi, dirStimRoot);
	connect(m_pmm, SIGNAL(agStarted()), this, SLOT(agStarted()));
	connect(m_pmm, SIGNAL(stimStarted(int)), this, SLOT(stimStarted(int)));
	connect(m_pmm, SIGNAL(stimulusChanged()), m_pVideoWidgetLeft->getHVideoWidget(), SLOT(stimulusChanged()));
	connect(m_pmm, SIGNAL(stimulusChanged()), m_pVideoWidgetRight->getHVideoWidget(), SLOT(stimulusChanged()));


	// l/r color b/g no sound
	if (flag.contains("all") || flag.contains("2"))
	{
		Habit::StimulusSettings s;
		s.setName("s1");
		s.setLeftStimulusInfo(Habit::StimulusInfo(Qt::blue));
		s.setRightStimulusInfo(Habit::StimulusInfo(Qt::green));
		Habit::StimulusInfo sis("mov/Loop.mov", true, 50);
		s.setIndependentSoundInfo(sis);
		f_stimuli.addStimulus(s);
	}

	// l/r color b/g iss
	if (flag.contains("all") || flag.contains("3"))
	{
		Habit::StimulusSettings s;
		s.setName("s2");
		s.setLeftStimulusInfo(Habit::StimulusInfo(Qt::red));
		s.setRightStimulusInfo(Habit::StimulusInfo(Qt::yellow));
		s.setIndependentSoundInfo(Habit::StimulusInfo(QString("sounds/ant-marching-band.mp3"), false, 50));
		f_stimuli.addStimulus(s);
	}

	// l/r color b/g iss
	if (flag.contains("all") || flag.contains("4"))
	{
		Habit::StimulusSettings s;
		s.setName("s4");
		s.setLeftStimulusInfo(Habit::StimulusInfo(Qt::cyan));
		s.setRightStimulusInfo(Habit::StimulusInfo(QString("anchovy"), QString("images/fish/anchovy.jpg")));
		f_stimuli.addStimulus(s);
	}

	// l/r color b/g iss
	if (flag.contains("all") || flag.contains("5"))
	{
		Habit::StimulusSettings s;
		s.setName("s5");
		s.setLeftStimulusInfo(Habit::StimulusInfo(QString("scad"), QString("images/fish/scad.jpg")));
		s.setRightStimulusInfo(Habit::StimulusInfo(Qt::magenta));
		s.setIndependentSoundInfo(Habit::StimulusInfo(QString("sounds/sad-trombone.wav"), true, 50));
		f_stimuli.addStimulus(s);
	}

	//
	if (flag.contains("all") || flag.contains("6"))
	{
		Habit::StimulusSettings s;
		s.setName("s6");
		s.setLeftStimulusInfo(Habit::StimulusInfo(QString("kami"), QString("mp4/pdreams-clip.mp4"), false, 50));
		s.setRightStimulusInfo(Habit::StimulusInfo(QString("SahyCheese"), QString("mp4/chees-clip.mp4"), false, 0));
		f_stimuli.addStimulus(s);
	}

	if (flag.contains("all") || flag.contains("7"))
	{
		Habit::StimulusSettings s;
		s.setName("s7");
		s.setLeftStimulusInfo(Habit::StimulusInfo(QString("kami"), QString("mp4/pdreams-clip.mp4"), false, 0));
		s.setRightStimulusInfo(Habit::StimulusInfo(QString("SahyCheese"), QString("mp4/pdreams-clip.mp4"), false, 50));
		f_stimuli.addStimulus(s);
	}

	if (flag.contains("all") || flag.contains("8"))
	{
		Habit::StimulusSettings s;
		s.setName("s8");
		s.setLeftStimulusInfo(Habit::StimulusInfo(QString("kami"), QString("mp4/pdreams-clip.mp4"), false, 0));
		s.setRightStimulusInfo(Habit::StimulusInfo(QString("SahyCheese"), QString("mp4/chees-clip.mp4"), false, 0));
		s.setIndependentSoundInfo(Habit::StimulusInfo(QString("sounds/welcome-to-the-internet.mp3"), true, 50));
		f_stimuli.addStimulus(s);
	}

	if (flag.contains("all") || flag.contains("9"))
	{
		Habit::StimulusSettings s;
		s.setName("s9");
		s.setLeftStimulusInfo(Habit::StimulusInfo(QString("chees-clip"), QString("mp4/chees-clip.mp4"), true, 50));
		s.setRightStimulusInfo(Habit::StimulusInfo(QString("pdreams-clip"), QString("mp4/pdreams-clip.mp4"), true, 50));
		f_stimuli.addStimulus(s);
	}

#if 0
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
#endif

	m_pmm->addStimuli(f_stimuli, 1);

//	m_pmm->preroll(0);
//	m_pmm->preroll(1);
//	m_pmm->stim(0);
	m_iCurrent = 0;

	//m_pmm->getReady(5000);
	return m_pmm->createStimulusWidget();
}


#if 0
void HG3Dialog::mediaChanged(libvlc_media_t *media)
{
	qInfo() << "media changed";
	// what can we find out about the media?

}
#endif

void HG3Dialog::started(int screenid, const QString& filename)
{
	qInfo() << "started " << screenid << ":" << filename;
}

void HG3Dialog::cleanupClicked()
{
	int i = m_sbWhich->value();
	qDebug() << "dump " << i;
	m_pmm->dump(i);
	qDebug() << "cleanup " << i;
	m_pmm->cleanup(i);
}

void HG3Dialog::initializeClicked()
{
	int i = m_sbWhich->value();
	qDebug() << "initialize " << i;
	m_pmm->initialize(i);
}

void HG3Dialog::prerollClicked()
{
	int i = m_sbWhich->value();
	qDebug() << "preroll " << i;
	m_pmm->preroll(i);
}

void HG3Dialog::rewindClicked()
{
	int i = m_sbWhich->value();
	qDebug() << "rewind " << i;
	m_pmm->rewind(i);
}

void HG3Dialog::pauseClicked()
{
	int i = m_sbWhich->value();
	qDebug() << "pause " << i;
	m_pmm->pause(i);
}

void HG3Dialog::playClicked()
{
	int i = m_sbWhich->value();
	qDebug() << "play " << i;
	m_pmm->stim(i);
}

void HG3Dialog::dumpClicked()
{
	int i = m_sbWhich->value();
	qDebug() << "dump " << i;
	m_pmm->dump(i);
}

void HG3Dialog::mmReady()
{
	qDebug() << "HG3Dialog::mmReady";
}

void HG3Dialog::mmFail()
{
	qDebug() << "HG3Dialog::mmFail";
}
