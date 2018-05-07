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
#include <QGst/Ui/VideoWidget>
#include "../../libs/habutil/HGstPlayer.h"
#include "../../libs/habutil/HGstMediaManager.h"

class Q5PlayerTestDialog: public QDialog
{
	Q_OBJECT

public:
	Q5PlayerTestDialog(const QDir& stimRootDir, int screen = -1, QWidget *parent = NULL);
	virtual ~Q5PlayerTestDialog();
protected Q_SLOTS:
	void playClicked();
	void showClicked();
	void started(int screenid, const QString& filename);
	//void mediaChanged(libvlc_media_t *media);

private:
	HGstMediaManager *m_phgstmm;
	//HGstPlayer *m_paudioHGstPlayer;
	//HGstPlayer *m_pvideoHGstPlayer;
	QGst::Ui::VideoWidget *m_pVideoWidget;
	QSpinBox *m_pSpinIndex;


	//HGstPlayer *createMediaPlayerAndPopulate(const QDir& dirStimRoot, bool isAudio);
	HGstMediaManager *createMM(QGst::Ui::VideoWidget *v);
};

#endif /* APPS_Q5PLAYER_Q5PLAYERTESTDIALOG_H_ */
