/*
 * HG2Dialog.h
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

class HG2Dialog: public QDialog
{
	Q_OBJECT

public:
	HG2Dialog(const QDir& stimRootDir, int screen = -1, bool bUseToggle=false, QWidget *parent = NULL);
	virtual ~HG2Dialog();
protected Q_SLOTS:
	void playClicked();
	void started(int screenid, const QString& filename);
	//void mediaChanged(libvlc_media_t *media);

private:
	//HGstMediaManager *m_phgstmm;
	//HGstPlayer *m_paudioHGstPlayer;
	//HGstPlayer *m_pvideoHGstPlayer;
	HGMM *m_pmm;
	QPushButton *m_pb;
	HStimulusWidget *m_pVideoWidgetLeft;
	HStimulusWidget *m_pVideoWidgetRight;
	HStimulusWidget *m_pVideoWidgetCenter;
	unsigned int m_iCurrent;

	QHBoxLayout *initSingleScreen(const Habit::StimulusDisplayInfo& sdi, const QDir& d);
	QHBoxLayout *initLRScreen(const Habit::StimulusDisplayInfo& sdi, const QDir& d);

	//HGstPlayer *createMediaPlayerAndPopulate(const QDir& dirStimRoot, bool isAudio);
//	HGstMediaManager *createMM(QGst::Ui::VideoWidget *v);

	public Q_SLOTS:

		void mmReady();
		void mmFail();
		void agStarted();
		void stimStarted(int);
};

#endif /* APPS_Q5PLAYER_Q5PLAYERTESTDIALOG_H_ */
