/*
 *  PlayerControlDialog.h
 *  myp
 *
 *  Created by Oakes Lab on 5/17/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <QtGui/QDialog>
#include <QtGui/QSpinBox>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QCheckBox>
#include <QList>
#include "HabitMediaManager.h"

class PlayerControlDialog : public QDialog
{
	Q_OBJECT
	
public:
	PlayerControlDialog(QWidget* w = 0);
	~PlayerControlDialog();
	
private:
	QPushButton* pbtnCreate;
	QSpinBox* pspinboxScreen;
	QCheckBox* pcheckboxShowImage;
	HabitMediaManager* pmm;
	QPushButton* pbtnPlay;
	QPushButton* pbtnStop;
	QPushButton* pbtnRewind;
	QPushButton* pbtnClear;
	QPushButton* pbtnNext;
	QPushButton* pbtnShow;
	QSpinBox* pspinboxShow;
	QList<QString> q;
	int qpos;
	int current;
	void nextSource();
	
private slots:
	void btnShowClicked();
	void btnCreateClicked();
	void btnStopClicked();
	void stimStarted();
	
signals:
	void playStim(int);
};
