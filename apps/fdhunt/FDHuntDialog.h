/*
 * FDHuntDialog.h
 *
 *  Created on: Dec 1, 2017
 *      Author: dan
 */

#ifndef APPS_FDHUNT_FDHUNTDIALOG_H_
#define APPS_FDHUNT_FDHUNTDIALOG_H_


#include <QtWidgets>
#include <gst/gst.h>
struct PipelineData;

class FDHuntDialog: public QDialog
{
	Q_OBJECT

public:
	FDHuntDialog(PipelineData *pdata, QWidget *parent = NULL);
	virtual ~FDHuntDialog();

	static void printPipelineState(GstElement *pipeline);

protected Q_SLOTS:
	void stateClicked();
	void pauseClicked();
	void playClicked();
	void rewindClicked();
	void cleanupClicked();

private:
	//HGstMediaManager *m_phgstmm;
	//HGstPlayer *m_paudioHGstPlayer;
	//HGstPlayer *m_pvideoHGstPlayer;
	PipelineData *m_pipelineData;
	QPushButton *m_pbState;
	QPushButton *m_pbPause;
	QPushButton *m_pbPlay;
	QPushButton *m_pbRewind;
	QPushButton *m_pbCleanup;
	//QSpinBox *m_sbWhich;


};

#endif /* APPS_FDHUNT_FDHUNTDIALOG_H_ */
