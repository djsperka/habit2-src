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
#include "HLooker.h"

class TestLookDialog: public QDialog
{
	Q_OBJECT
	
public:
	
	TestLookDialog(QWidget* parent=0);
	~TestLookDialog() {};

protected:
	bool eventFilter ( QObject * o, QEvent * e );
private:
	bool m_bGoClicked;
	QLineEdit* lineeditLook;
	QLineEdit* lineeditLookAway;
	QPushButton* pushbuttonGo;
	QListWidget* listWidget;
	HLooker* looker;
	
private slots:
	void goClicked();	
	void gotLook(HLook);
	
};

