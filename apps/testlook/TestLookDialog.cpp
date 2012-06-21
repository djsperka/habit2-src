/*
 *  TestLookDialog.cpp
 *  testlook
 *
 *  Created by Oakes Lab on 6/19/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "TestLookDialog.h"
#include <QTextStream>
#include <QIntValidator>
#include <QCoreApplication>

TestLookDialog::TestLookDialog(QWidget* parent) : QDialog(parent), m_bGoClicked(false)
{
	pushbuttonGo = new QPushButton();
	pushbuttonGo->setText("Go");
	QVBoxLayout *layout = new QVBoxLayout(this);
	lineeditLook = new QLineEdit("1000");
	lineeditLook->setValidator(new QIntValidator(1, 10000));
	lineeditLookAway = new QLineEdit("250");
	lineeditLookAway->setValidator(new QIntValidator(0, 10000));
	listWidget = new QListWidget(this);
	layout->addWidget(lineeditLook);
	layout->addWidget(lineeditLookAway);
	layout->addWidget(pushbuttonGo);
	layout->addWidget(listWidget);

	connect(pushbuttonGo, SIGNAL(clicked()), this, SLOT(goClicked()));
	QCoreApplication::instance()->installEventFilter(this);
};


void TestLookDialog::goClicked()
{
	bool bValue;
	qDebug() << "goClicked()" << lineeditLook->text().toInt(&bValue) <<  " " << lineeditLookAway->text().toInt(&bValue) << endl;
	pushbuttonGo->setDisabled(true);
	lineeditLook->setDisabled(true);
	lineeditLookAway->setDisabled(true);
	//listWidget->setDisabled(true);
	m_bGoClicked = true;
	//listWidget->installEventFilter(this);
	
	looker = new HLooker(lineeditLook->text().toInt(&bValue), lineeditLookAway->text().toInt(&bValue));
	connect(looker, SIGNAL(look(HLook)), this, SLOT(gotLook(HLook)));
	return;
};

void TestLookDialog::gotLook(HLook l)
{
	QString s;
	QTextStream out(&s);
	out << l;
	qDebug() << "Got look signal " << s;
	new QListWidgetItem(s, listWidget);
}

bool TestLookDialog::eventFilter(QObject *obj, QEvent *event)
{
	QTime t = QTime::currentTime();
	QKeyEvent *keyEvent;
	bool bVal = false;
	if (m_bGoClicked) 
	{
		switch (event->type()) 
		{
			case QEvent::KeyPress:
				keyEvent = static_cast<QKeyEvent*>(event);
				if (keyEvent->isAutoRepeat())
				{
					bVal = true;
					//qDebug() << "Ignore auto-repeat " << keyEvent->key();
				}
				else 
				{
					qDebug() << "Ate key press " << keyEvent->key();
					switch (keyEvent->key()) 
					{
						case Qt::Key_4:
							looker->addTrans(NoneLeft, t);
							bVal = true;
							break;
						case Qt::Key_6:
							looker->addTrans(NoneRight, t);
							bVal = true;
							break;
						default:
							break;
					}
				}
				break;
			case QEvent::KeyRelease:
				keyEvent = static_cast<QKeyEvent*>(event);
				qDebug() << "Ate key release " << keyEvent->key();
				switch (keyEvent->key()) {
					case Qt::Key_4:
						looker->addTrans(LeftNone, t);
						bVal = true;
						break;
					case Qt::Key_6:
						looker->addTrans(RightNone, t);
						bVal = true;
						break;
					default:
						break;
				}
				break;
			default:
				break;
			}
		}
		else 
		{
			// pass the event on to the parent class
			bVal = QDialog::eventFilter(obj, event);
		}
	return bVal;
}