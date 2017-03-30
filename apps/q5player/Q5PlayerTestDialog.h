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
#include "HAudioPlayer5.h"
#include "HVIPlayer5.h"

class Q5PlayerTestDialog: public QDialog
{
	Q_OBJECT
public:
	Q5PlayerTestDialog(const QDir& stimRootDir, QWidget *parent = NULL);
	virtual ~Q5PlayerTestDialog();
protected slots:
	void playClicked();

private:
	HAudioPlayer5 *m_paudio;
	HVIPlayer5 *m_pvi;

	HAudioPlayer5 *createPlayer(const QDir& dirStimRoot);
	HVIPlayer5 *createVIPlayer(const QDir& dirStimRoot);
};

#endif /* APPS_Q5PLAYER_Q5PLAYERTESTDIALOG_H_ */
