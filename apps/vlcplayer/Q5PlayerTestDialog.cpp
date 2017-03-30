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
#include "../../libs/habutil/HVLCMediaPlayer.h"
#include "../../libs/habutil/HVLCVideoWidget.h"

#define MAX_INFO 5
Habit::StimulusInfo f_vinfo[MAX_INFO];
Habit::StimulusInfo f_ainfo[MAX_INFO];

Q5PlayerTestDialog::Q5PlayerTestDialog(const QDir& dirStimRoot, int screen, QWidget *parent)
: QDialog(parent)
, m_paudioHVLCMediaPlayer(NULL)
, m_pvideoHVLCMediaPlayer(NULL)
, m_pHVLCVideoWidget(NULL)
{
	m_paudioHVLCMediaPlayer = createMediaPlayerAndPopulate(dirStimRoot, true);
	m_pvideoHVLCMediaPlayer = createMediaPlayerAndPopulate(dirStimRoot, false);
	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel, Qt::Horizontal, parent);
	QPushButton *pb = new QPushButton("Play");
	QPushButton *pbshow = new QPushButton("Show");
	QVBoxLayout *vbox = new QVBoxLayout;

	buttonBox->addButton(pb, QDialogButtonBox::ActionRole);
	connect(pb, SIGNAL(clicked()), this, SLOT(playClicked()));
	buttonBox->addButton(pbshow, QDialogButtonBox::ActionRole);
	connect(pbshow, SIGNAL(clicked()), this, SLOT(showClicked()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(accept()));
	connect(m_paudioHVLCMediaPlayer, SIGNAL(started(int, const QString&)), this, SLOT(started(int, const QString& )));
	connect(m_pvideoHVLCMediaPlayer, SIGNAL(started(int, const QString&)), this, SLOT(started(int, const QString& )));

	m_pSpinIndex = new QSpinBox(this);
	m_pSpinIndex->setMinimum(100);
	m_pSpinIndex->setMaximum(104);

	// Create video widget, and assign it as output for viplayer

	if (screen < 0)
	{
		m_pHVLCVideoWidget = new HVLCVideoWidget(m_pvideoHVLCMediaPlayer, this, QColor(Qt::blue), QSize(320, 240));
#if 0
		m_pWidgetVideo = new VlcWidgetVideo(this, QColor(Qt::red));
		m_pvideo->vlcPlayer()->setVideoWidget(m_pWidgetVideo);
		m_pWidgetVideo->setMinimumSize(320, 240);
#endif
		vbox->addWidget(m_pHVLCVideoWidget);
		vbox->addWidget(m_pSpinIndex);
		vbox->addWidget(buttonBox);
		setLayout(vbox);

		// NOTE: YOU MUST SHOW() IT or it will not appear!
		m_pHVLCVideoWidget->show();

		//connect(m_pvideo->vlcPlayer(), SIGNAL(mediaChanged(libvlc_media_t *media)), this, SLOT(mediaChanged(libvlc_media_t *media)));
		// QObject::connect: No such signal VlcMediaPlayer::mediaChanged(libvlc_media_t *media)

		connect(m_pvideoHVLCMediaPlayer->vlcMediaPlayer(), SIGNAL(mediaChanged(libvlc_media_t *)), this, SLOT(mediaChanged(libvlc_media_t *)));
		connect(m_pvideoHVLCMediaPlayer, SIGNAL(sizeChanged(unsigned int, unsigned int)), m_pHVLCVideoWidget, SLOT(sizeChanged(unsigned int, unsigned int)));
	}
	else
	{
		// TODO fix this see commented line
		// Note in full screen case there is no parent!
		m_pHVLCVideoWidget = new HVLCVideoWidget(m_pvideoHVLCMediaPlayer, 1, QColor(Qt::blue));
		//FIX m_pvideo->vlcPlayer()->setVideoWidget(m_pVideoWidget);
		vbox->addWidget(m_pSpinIndex);
		vbox->addWidget(buttonBox);
		setLayout(vbox);

		qInfo() << "Set geometry to " << QApplication::desktop()->screenGeometry(screen);

		m_pHVLCVideoWidget->setGeometry(QApplication::desktop()->screenGeometry(screen));
		m_pHVLCVideoWidget->setWindowFlags(m_pHVLCVideoWidget->windowFlags() | Qt::FramelessWindowHint);
		//Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
		//m_pVideoWidget->setWindowState(m_pVideoWidget->windowState() | Qt::WindowFullScreen);
		//			player->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
		m_pHVLCVideoWidget->show();	// have to show it or it won't be visible

		connect(m_pvideoHVLCMediaPlayer->vlcMediaPlayer(), SIGNAL(mediaChanged(libvlc_media_t *)), this, SLOT(mediaChanged(libvlc_media_t *)));
		connect(m_pvideoHVLCMediaPlayer, SIGNAL(sizeChanged(unsigned int, unsigned int)), m_pHVLCVideoWidget, SLOT(sizeChanged(unsigned int, unsigned int)));

	}
}

Q5PlayerTestDialog::~Q5PlayerTestDialog()
{
	delete m_paudioHVLCMediaPlayer;
	delete m_pvideoHVLCMediaPlayer;

}

void Q5PlayerTestDialog::mediaChanged(libvlc_media_t *media)
{
	qInfo() << "media changed";
	// what can we find out about the media?

}

void Q5PlayerTestDialog::started(int screenid, const QString& filename)
{
	qInfo() << "started " << screenid << ":" << filename;
}

void Q5PlayerTestDialog::playClicked()
{
	m_paudioHVLCMediaPlayer->play(m_pSpinIndex->value());
	m_pvideoHVLCMediaPlayer->play(m_pSpinIndex->value());
}

void Q5PlayerTestDialog::showClicked()
{
	if (m_pHVLCVideoWidget->isVisible())
		m_pHVLCVideoWidget->hide();
	else
		m_pHVLCVideoWidget->show();
}


HVLCMediaPlayer *Q5PlayerTestDialog::createMediaPlayerAndPopulate(const QDir& dirStimRoot, bool isAudio)
{
	HVLCMediaPlayer *player = NULL;

	// HVLCMediaPlayer *Q5PlayerTestDialog::createPlayer(const QDir& dirStimRoot)
	if (isAudio)
	{
		player = new HVLCMediaPlayer(0, NULL, dirStimRoot);

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
		player = new HVLCMediaPlayer(1, NULL, dirStimRoot);

		f_vinfo[0].setFileName(QString("mov/butterfly.mp4"));
		f_vinfo[0].setVolume(0);
		f_vinfo[0].setLoopPlayBack(false);
		player->addStimulus(100, f_vinfo[0]);

		f_vinfo[1].setFileName(QString("mov/butterfly-720p.mov"));
		f_vinfo[1].setVolume(0);
		f_vinfo[1].setLoopPlayBack(false);
		player->addStimulus(101, f_vinfo[1]);

		f_vinfo[2].setFileName(QString("images/fish/anchovy.jpg"));
		f_vinfo[2].setVolume(0);
		f_vinfo[2].setLoopPlayBack(false);
		player->addStimulus(102, f_vinfo[2]);

		f_vinfo[3].setFileName(QString("images/letters/C.jpg"));
		f_vinfo[3].setVolume(0);
		f_vinfo[3].setLoopPlayBack(false);
		player->addStimulus(103, f_vinfo[3]);

		f_vinfo[4].setFileName(QString("images/posters/poultry.jpg"));
		f_vinfo[4].setVolume(0);
		f_vinfo[4].setLoopPlayBack(false);
		player->addStimulus(104, f_vinfo[4]);

	}
	return player;
}

