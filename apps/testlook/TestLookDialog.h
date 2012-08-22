/*
 *  TestLookDialog.h
 *  testlook
 *
 *  Created by Oakes Lab on 6/19/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <QtGui>
#include <QKeyEvent>
#include <QTime>
#include "HKeypadLookDetector.h"
//#include "HLooker.h"

class TestLookDialog: public QDialog
{
	Q_OBJECT
	
public:
	
	TestLookDialog(QWidget* parent=0);
	~TestLookDialog() {};

protected:
private:
	bool m_bGoClicked;
	QTime m_time;
	QLineEdit* lineeditLook;
	QLineEdit* lineeditLookAway;
	QRadioButton* rbAG;
	QRadioButton* rbLook;
	QPushButton* pushbuttonGo;
	QListWidget* listWidget;
	//HLooker* looker;
	HLookDetector* ld;
private slots:
	void goClicked();	
	void gotLook(HLook);
	void gotAttention();
	void agToggled(bool);
	void lookToggled(bool);
};

