/*
 * HG3Dialog.h
 *
 *  Created on: Feb 23, 2017
 *      Author: dan
 */

#ifndef APPS_Q5PLAYER_Q5PLAYERTESTDIALOG_H_
#define APPS_Q5PLAYER_Q5PLAYERTESTDIALOG_H_

#include <hgst/HStimulusWidget.h>

#include <QtWidgets>
#include <QDir>
#include "HStimulusWidget.h"
#include "HGMM.h"
//#include "../../libs/habutil/HMediaManager.h"


class HG3Dialog: public QDialog
{
	Q_OBJECT

public:
	HG3Dialog(const QDir& stimRootDir, int screen = -1, const QString& flag = QString(""), QWidget *parent = NULL);
	virtual ~HG3Dialog();
protected Q_SLOTS:
	void readyClicked();
	void prerollClicked();
	void pauseClicked();
	void playClicked();
	void rewindClicked();
	void started(int screenid, const QString& filename);
	//void mediaChanged(libvlc_media_t *media);

private:
	//HGstMediaManager *m_phgstmm;
	//HGstPlayer *m_paudioHGstPlayer;
	//HGstPlayer *m_pvideoHGstPlayer;
	HGMM *m_pmm;
	QPushButton *m_pbReady;
	QPushButton *m_pbPreroll;
	QPushButton *m_pbPause;
	QPushButton *m_pbPlay;
	QPushButton *m_pbRewind;
	QSpinBox *m_sbWhich;
	HStimulusWidget *m_pVideoWidgetLeft;
	HStimulusWidget *m_pVideoWidgetRight;
	HStimulusWidget *m_pVideoWidgetCenter;
	unsigned int m_iCurrent;

	QHBoxLayout *initSingleScreen(const Habit::StimulusDisplayInfo& sdi, const QDir& d, const QString& flag);
	QHBoxLayout *initLRScreen(const Habit::StimulusDisplayInfo& sdi, const QDir& d, const QString& flag);

	//HGstPlayer *createMediaPlayerAndPopulate(const QDir& dirStimRoot, bool isAudio);
//	HGstMediaManager *createMM(QGst::Ui::VideoWidget *v);

	public Q_SLOTS:

		void mmReady();
		void mmFail();
		void agStarted();
		void stimStarted(int);
};

#endif /* APPS_Q5PLAYER_Q5PLAYERTESTDIALOG_H_ */
