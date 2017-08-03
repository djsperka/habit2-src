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

#define MAX_INFO 5
Habit::StimulusInfo f_vinfo[MAX_INFO];
Habit::StimulusInfo f_ainfo[MAX_INFO];
Habit::StimuliSettings f_stimuli;

HG2Dialog::HG2Dialog(const QDir& dirStimRoot, int screen, QWidget *parent)
: QDialog(parent)
, m_pVideoWidgetLeft(NULL)
, m_pVideoWidgetRight(NULL)
, m_pVideoWidgetCenter(NULL)
{
	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel, Qt::Horizontal, parent);
	QPushButton *pb = new QPushButton("Play");
	QVBoxLayout *vbox = new QVBoxLayout;
	buttonBox->addButton(pb, QDialogButtonBox::ActionRole);
	connect(pb, SIGNAL(clicked()), this, SLOT(playClicked()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(accept()));
	if (screen == 1)
	{
		vbox->addLayout(initSingleScreen(dirStimRoot));
	}
	else
	{
		vbox->addLayout(initLRScreen(dirStimRoot));
	}
	m_pSpinIndex = new QSpinBox(this);
	m_pSpinIndex->setMinimum(0);
	m_pSpinIndex->setMaximum(12);

	vbox->addWidget(m_pSpinIndex);
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

QHBoxLayout *HG2Dialog::initSingleScreen(const QDir& dirStimRoot)
{
	QHBoxLayout *hbox = new QHBoxLayout;
	m_pVideoWidgetCenter = new QGst::Ui::VideoWidget(this);
	m_pVideoWidgetCenter->setMinimumSize(320, 240);

	hbox->addWidget(m_pVideoWidgetCenter);

	m_pmm = new HGMM(m_pVideoWidgetCenter, dirStimRoot);
	connect(m_pmm, SIGNAL(mmReady()), this, SLOT(mmReady()));
	connect(m_pmm, SIGNAL(mmFail()), this, SLOT(mmFail()));
	connect(m_pmm, SIGNAL(agStarted()), this, SLOT(agStarted()));
	connect(m_pmm, SIGNAL(stimStarted(int)), this, SLOT(stimStarted(int)));

	// create background
	m_pmm->addBackground(QColor(Qt::green));

	// pic
	Habit::StimulusSettings s1;
	s1.setName("s1");
	Habit::StimulusInfo si1(QString("oceanz"), QString("images/posters/ocean.jpg"));
	s1.setCenterStimulusInfo(si1);
	f_stimuli.addStimulus(s1);

	// movie
	Habit::StimulusSettings s2;
	s2.setName("s2");
	Habit::StimulusInfo si2(QString("earthsunz"), QString("mov/earthsun.mov"), false);
	s2.setCenterStimulusInfo(si2);
	f_stimuli.addStimulus(s2);

	Habit::StimulusSettings s3;
	s3.setName("s3");
	Habit::StimulusInfo si3(QString("taxi"), QString("mov/taxi-480p.mov"), true);
	s3.setCenterStimulusInfo(si3);
	f_stimuli.addStimulus(s3);

	Habit::StimulusSettings s4;
	s4.setName("s4");
	Habit::StimulusInfo si4(QString("hydrant"), QString("images/tools/hydrant.png"));
	s4.setCenterStimulusInfo(si4);
	f_stimuli.addStimulus(s4);

	m_pmm->addStimuli(f_stimuli, 1);

	m_pmm->getReady(5000);

	return hbox;
}


QHBoxLayout *HG2Dialog::initLRScreen(const QDir& dirStimRoot)
{
	QHBoxLayout *hbox = new QHBoxLayout;
	m_pVideoWidgetLeft = new QGst::Ui::VideoWidget(this);
	m_pVideoWidgetLeft->setMinimumSize(320, 240);
	m_pVideoWidgetRight = new QGst::Ui::VideoWidget(this);
	m_pVideoWidgetRight->setMinimumSize(320, 240);
	hbox->addWidget(m_pVideoWidgetLeft);
	hbox->addWidget(m_pVideoWidgetRight);

	m_pmm = new HGMM(m_pVideoWidgetLeft, m_pVideoWidgetRight, dirStimRoot);
	connect(m_pmm, SIGNAL(mmReady()), this, SLOT(mmReady()));
	connect(m_pmm, SIGNAL(mmFail()), this, SLOT(mmFail()));
	connect(m_pmm, SIGNAL(agStarted()), this, SLOT(agStarted()));
	connect(m_pmm, SIGNAL(stimStarted(int)), this, SLOT(stimStarted(int)));

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

	m_pmm->getReady(5000);
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
	//m_pvideoHGstPlayer->play(m_pSpinIndex->value());
	qDebug() << "Play " << m_pSpinIndex->value();
	m_pmm->stim(m_pSpinIndex->value());
}

void HG2Dialog::mmReady()
{
	qDebug() << "HG2Dialog::mmReady";
}

void HG2Dialog::mmFail()
{
	qDebug() << "HG2Dialog::mmFail";
}
