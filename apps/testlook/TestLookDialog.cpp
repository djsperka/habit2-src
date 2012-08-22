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
	m_time.start();
	pushbuttonGo = new QPushButton();
	pushbuttonGo->setText("Go");
	QVBoxLayout *layout = new QVBoxLayout(this);
	lineeditLook = new QLineEdit("1000");
	lineeditLook->setValidator(new QIntValidator(1, 10000));
	lineeditLookAway = new QLineEdit("250");
	lineeditLookAway->setValidator(new QIntValidator(0, 10000));
	rbAG = new QRadioButton("AG");
	rbLook = new QRadioButton("Look");
	listWidget = new QListWidget(this);
	layout->addWidget(lineeditLook);
	layout->addWidget(lineeditLookAway);
	layout->addWidget(rbAG);
	layout->addWidget(rbLook);
	layout->addWidget(pushbuttonGo);
	layout->addWidget(listWidget);

	connect(pushbuttonGo, SIGNAL(clicked()), this, SLOT(goClicked()));
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
	
	//looker = new HLooker(lineeditLook->text().toInt(&bValue), lineeditLookAway->text().toInt(&bValue));
	ld = new HKeypadLookDetector(lineeditLook->text().toInt(&bValue), lineeditLookAway->text().toInt(&bValue),
								 listWidget, true, true, true);
	if (rbAG->isChecked())
	{
		ld->enableAGLook();
	}
	if (rbLook->isChecked())
	{
		ld->enableLook();
	}
	//connect(looker, SIGNAL(look(HLook)), this, SLOT(gotLook(HLook)));
	connect(ld, SIGNAL(look(HLook)), this, SLOT(gotLook(HLook)));
	connect(ld, SIGNAL(attention()), this, SLOT(gotAttention()));
	connect(rbAG, SIGNAL(toggled(bool)), this, SLOT(agToggled(bool)));
	connect(rbLook, SIGNAL(toggled(bool)), this, SLOT(lookToggled(bool)));
	return;
};

void TestLookDialog::agToggled(bool checked)
{
	if (checked) ld->enableAGLook();
}

void TestLookDialog::lookToggled(bool checked)
{
	if (checked) ld->enableLook();
}

void TestLookDialog::gotLook(HLook l)
{
	QString s;
	QTextStream out(&s);
	out << l;
	qDebug() << "Got look signal " << s;
	new QListWidgetItem(s, listWidget);
}

void TestLookDialog::gotAttention()
{
	QString s("Got attention signal");
	qDebug() << s;
	new QListWidgetItem(s, listWidget);
}	

