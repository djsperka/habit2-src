/*
 *  TestTrialDialog.h
 *  testtrial
 *
 *  Created by Oakes Lab on 6/19/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _TESTTRIALDIALOG_H_
#define _TESTTRIALDIALOG_H_

#include <QtGui>
#include <QKeyEvent>
#include <QTime>
#include "HLooker.h"
#include "HTrial.h"
#include "HabitMediaManager.h"

class TestTrialDialog: public QDialog
{
	Q_OBJECT
	
public:
	
	TestTrialDialog(QWidget* parent=0);
	~TestTrialDialog() {};
	
protected:
	bool eventFilter ( QObject * o, QEvent * e );
private:
	bool m_bGoClicked;
	QTime m_time;
	QLineEdit* lineeditLook;
	QLineEdit* lineeditLookAway;
	QPushButton* pushbuttonGo;
	QListWidget* listWidget;
	QLineEdit* lineeditMaxMS;
	QCheckBox* checkboxAG;
	QCheckBox* checkboxFixed;

	QGroupBox* createTrialSettingsGroup();
	HabitMediaManager* createMediaManager();

	HLooker* looker;
	HTrial* trial;
	
	
	private slots:
	void goClicked();	
	void gotLook(HLook);
	
};
#endif
