/*
 * Q5PlayerTestDialog.h
 *
 *  Created on: Feb 23, 2017
 *      Author: dan
 */

#ifndef APPS_Q5PLAYER_Q5PLAYERTESTDIALOG_H_
#define APPS_Q5PLAYER_Q5PLAYERTESTDIALOG_H_

#include <QtWidgets>
#include <QDir>
#include <VLCQtWidgets/WidgetVideo.h>
#include "../../libs/habutil/HVLCMediaPlayer.h"
#include "../../libs/habutil/HVLCVideoWidget.h"

class Q5PlayerTestDialog: public QDialog
{
	Q_OBJECT

public:
	Q5PlayerTestDialog(const QDir& stimRootDir, int screen = -1, QWidget *parent = NULL);
	virtual ~Q5PlayerTestDialog();
protected slots:
	void playClicked();
	void showClicked();
	void started(int screenid, const QString& filename);
	void mediaChanged(libvlc_media_t *media);

private:
	HVLCMediaPlayer *m_paudioHVLCMediaPlayer;
	//HVLCVideoPlayer5 *m_pvideo;
	HVLCMediaPlayer *m_pvideoHVLCMediaPlayer;
	HVLCVideoWidget *m_pHVLCVideoWidget;
	QSpinBox *m_pSpinIndex;

	HVLCMediaPlayer *createMediaPlayerAndPopulate(const QDir& dirStimRoot, bool isAudio);
};

#endif /* APPS_Q5PLAYER_Q5PLAYERTESTDIALOG_H_ */
