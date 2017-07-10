/*
 * Q5PlayerTestDialog.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: dan
 */

#include "Q5PlayerTestDialog.h"
#include <QDialogButtonBox>
#include <QDesktopWidget>
#include <QtDebug>

#include "stimulisettings.h"

#define MAX_INFO 5
Habit::StimulusInfo f_vinfo[MAX_INFO];
Habit::StimulusInfo f_ainfo[MAX_INFO];
Habit::StimuliSettings f_stimuli;

Q5PlayerTestDialog::Q5PlayerTestDialog(const QDir& dirStimRoot, int screen, QWidget *parent)
: QDialog(parent)
, m_pVideoWidget(NULL)
{
	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel, Qt::Horizontal, parent);
	QPushButton *pb = new QPushButton("Play");
	QPushButton *pbshow = new QPushButton("Show");
	QVBoxLayout *vbox = new QVBoxLayout;

	buttonBox->addButton(pb, QDialogButtonBox::ActionRole);
	connect(pb, SIGNAL(clicked()), this, SLOT(playClicked()));
	buttonBox->addButton(pbshow, QDialogButtonBox::ActionRole);
	connect(pbshow, SIGNAL(clicked()), this, SLOT(showClicked()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(accept()));
	//connect(m_paudioHGstPlayer, SIGNAL(started(int, const QString&)), this, SLOT(started(int, const QString& )));

	m_pSpinIndex = new QSpinBox(this);
	m_pSpinIndex->setMinimum(0);
	m_pSpinIndex->setMaximum(4);

	// Create video widget, and assign it as output for viplayer

	if (screen < 0)
	{
		m_pVideoWidget = new QGst::Ui::VideoWidget(this);
		m_pVideoWidget->setMinimumSize(320, 240);
		vbox->addWidget(m_pVideoWidget);
		vbox->addWidget(m_pSpinIndex);
		vbox->addWidget(buttonBox);
		setLayout(vbox);

		m_phgstmm = createMM(m_pVideoWidget);
		//connect(m_pvideoHGstPlayer, SIGNAL(started(int, const QString&)), this, SLOT(started(int, const QString& )));

		m_pVideoWidget->show();
	}
}

Q5PlayerTestDialog::~Q5PlayerTestDialog()
{
	//delete m_paudioHGstPlayer;
	//m_pvideoHGstPlayer->stop();
	//delete m_pvideoHGstPlayer;
}

#if 0
void Q5PlayerTestDialog::mediaChanged(libvlc_media_t *media)
{
	qInfo() << "media changed";
	// what can we find out about the media?

}
#endif

void Q5PlayerTestDialog::started(int screenid, const QString& filename)
{
	qInfo() << "started " << screenid << ":" << filename;
}

void Q5PlayerTestDialog::playClicked()
{
	//m_pvideoHGstPlayer->play(m_pSpinIndex->value());
	qDebug() << "Play " << m_pSpinIndex->value();
}

void Q5PlayerTestDialog::showClicked()
{
#if 0
	if (m_pHVLCVideoWidget->isVisible())
		m_pHVLCVideoWidget->hide();
	else
		m_pHVLCVideoWidget->show();
#endif
}

HGstMediaManager *Q5PlayerTestDialog::createMM(QGst::Ui::VideoWidget *v)
{
	HGstMediaManager *pmm = new HGstMediaManager(HStimulusLayoutType::HStimulusLayoutSingle, v, false);

#if 0
	// background
	Habit::StimulusSettings s0;
	s0.setName("s0");
	Habit::StimulusInfo si0("name", "", false, 0, true, false, QColor(Qt::gray));
	s0.setCenterStimulusInfo(si0);

	f_stimuli.addStimulus(s0);
#endif

	// pic
	Habit::StimulusSettings s1;
	s1.setName("s1");
	Habit::StimulusInfo si1("name", "images/posters/ocean.jpg");
	s1.setCenterStimulusInfo(si1);
	f_stimuli.addStimulus(s1);

	// movie
	Habit::StimulusSettings s2;
	s2.setName("s2");
	Habit::StimulusInfo si2("name", "mov/earthsun.mov");
	s2.setCenterStimulusInfo(si2);
	f_stimuli.addStimulus(s2);

	// now populate mm
	QListIterator<Habit::StimulusSettings> it(f_stimuli.stimuli());
	while (it.hasNext())
	{
		unsigned int n = pmm->addStimulus(it.next());
		qDebug() << "added stim, id " << n;
	}

	return pmm;
}



#if 0
HGstPlayer *Q5PlayerTestDialog::createMediaPlayerAndPopulate(const QDir& dirStimRoot, bool isAudio)
{
	HGstPlayer *player = NULL;
#if 0
	// HVLCMediaPlayer *Q5PlayerTestDialog::createPlayer(const QDir& dirStimRoot)
	if (isAudio)
	{
		player = new HGstPlayer(0, NULL, dirStimRoot);

		f_ainfo[0].setVolume(75);
		f_ainfo[0].setLoopPlayBack(false);
		f_ainfo[0].setIsBackground(false);
		f_ainfo[0].setFileName(QString("wav/fanfare_x.wav"));
		player->addStimulus(100, f_ainfo[0]);

		// 101 = no audio

		f_ainfo[2].setVolume(50);
		f_ainfo[2].setLoopPlayBack(false);
		f_ainfo[2].setIsBackground(false);
		f_ainfo[2].setFileName(QString("wav/bowling.wav"));
		player->addStimulus(102, f_ainfo[2]);

		f_ainfo[3].setVolume(100);
		f_ainfo[3].setLoopPlayBack(false);
		f_ainfo[3].setIsBackground(false);
		f_ainfo[3].setFileName(QString("wav/doorbell_x.wav"));
		player->addStimulus(103, f_ainfo[3]);

		f_ainfo[4].setVolume(90);
		f_ainfo[4].setLoopPlayBack(false);
		f_ainfo[4].setIsBackground(false);
		f_ainfo[4].setFileName(QString("wav/cuckoo_clock2_x.wav"));
		player->addStimulus(104, f_ainfo[4]);
	}
	else
	{
#else
		player = new HGstPlayer(1, NULL, dirStimRoot);

		f_vinfo[0].setFileName(QString("images/posters/ocean.jpg"));
		f_vinfo[0].setVolume(0);
		f_vinfo[0].setLoopPlayBack(false);
		player->addStimulus(1, f_vinfo[0]);

		//f_vinfo[1].setFileName(QString("mov/earthsun.mov"));
		f_vinfo[1].setFileName(QString("images/fish/anchovy.jpg"));
		f_vinfo[1].setVolume(0);
		f_vinfo[1].setLoopPlayBack(false);
		player->addStimulus(2, f_vinfo[1]);

		//f_vinfo[2].setFileName(QString("mov/butterfly.mp4"));
		f_vinfo[2].setFileName(QString("images/letters/M.jpg"));
		f_vinfo[2].setVolume(0);
		f_vinfo[2].setLoopPlayBack(false);
		player->addStimulus(3, f_vinfo[2]);

		//f_vinfo[3].setFileName(QString("wav/doorbell_x.wav"));
		f_vinfo[3].setFileName(QString("images/posters/poultry.jpg"));
		f_vinfo[3].setVolume(0);
		f_vinfo[3].setLoopPlayBack(false);
		player->addStimulus(4, f_vinfo[3]);

#endif
	return player;
}

#endif
