/*
 * q5player.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: dan
 */

#include <QApplication>
#include <QtDebug>
#include <QMessageBox>
#include <QDir>
#include <QDesktopWidget>

//#include "HAudioPlayer5.h"
#include "Q5PlayerTestDialog.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	int screen = -1;

	app.setApplicationName("hvlcplayer");
	app.setOrganizationName("Infant Cognition Lab");
	app.setOrganizationDomain("infantcognitionlab.ucdavis.edu");

	qInfo() << "Number of screens: " << QApplication::desktop()->screenCount();
	for (int i=0; i<QApplication::desktop()->screenCount(); i++)
	{
		qInfo() << "screen " << i << ": screenGeometry: " << QApplication::desktop()->screenGeometry(i);
		qInfo() << "screen " << i << ": availableGeometry: " << QApplication::desktop()->availableGeometry(i);
	}

	if (argc == 2)
		screen = QString(argv[1]).toInt();
	screen = 1;
	Q5PlayerTestDialog dialog(QDir("/Users/dan/Desktop/habit22/stim/examples"), screen);

	dialog.exec();
//	QMessageBox msgBox;
//	msgBox.setText("The document has been modified.");
//	msgBox.exec();
	return 0;
}



