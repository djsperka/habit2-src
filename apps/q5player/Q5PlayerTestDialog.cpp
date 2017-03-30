/*
 * Q5PlayerTestDialog.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: dan
 */

#include "Q5PlayerTestDialog.h"
#include <QDialogButtonBox>
#include <QVideoWidget>

Habit::StimulusInfo f_info;
Habit::StimulusInfo f_vinfo;
Habit::StimulusInfo f_iinfo;

Q5PlayerTestDialog::Q5PlayerTestDialog(const QDir& dirStimRoot, QWidget *parent)
: QDialog(parent)
, m_paudio(NULL)
{
	m_paudio = createPlayer(dirStimRoot);
	m_pvi = createVIPlayer(dirStimRoot);
	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel, Qt::Horizontal, parent);
	QPushButton *pb = new QPushButton("Play");
	QVBoxLayout *vbox = new QVBoxLayout;

	buttonBox->addButton(pb, QDialogButtonBox::ActionRole);
	connect(pb, SIGNAL(clicked()), this, SLOT(playClicked()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(accept()));


	// Create video widget, and assign it as output for viplayer
	QVideoWidget *vw = new QVideoWidget;
	m_pvi->setVideoOutput(vw);
	vbox->addWidget(vw);
	vbox->addWidget(buttonBox);
	vw->show();
	setLayout(vbox);
}

Q5PlayerTestDialog::~Q5PlayerTestDialog()
{
}

void Q5PlayerTestDialog::playClicked()
{
	m_pvi->play(100);
	m_paudio->play(100);
}

HAudioPlayer5 *Q5PlayerTestDialog::createPlayer(const QDir& dirStimRoot)
{
	HAudioPlayer5 *player = new HAudioPlayer5(0, this, dirStimRoot);
	f_info.setVolume(75);
	f_info.setLoopPlayBack(false);
	f_info.setIsBackground(false);
	f_info.setFileName(QString("wav/fanfare_x.wav"));
	player->addStimulus(100, f_info);
	return player;
}

HVIPlayer5 *Q5PlayerTestDialog::createVIPlayer(const QDir& d)
{
	HVIPlayer5 *player = new HVIPlayer5(1, this, d);
	//f_vinfo.setFileName(QString("mov/butterfly-720p.mov"));
	f_vinfo.setFileName(QString("mov/butterfly.mp4"));
	//f_vinfo.setFileName(QString("images/fish/anchovy.jpg"));
	f_vinfo.setVolume(0);
	f_vinfo.setLoopPlayBack(false);
	player->addStimulus(100, f_vinfo);
	return player;
}
