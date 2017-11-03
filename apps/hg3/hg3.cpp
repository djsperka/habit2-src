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
#include <gst/gst.h>


//#include "HAudioPlayer5.h"
#include "HG3Dialog.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	gst_init(&argc, &argv);
	int screen = -1;

	app.setApplicationName("hgstplayer");
	app.setOrganizationName("Infant Cognition Lab");
	app.setOrganizationDomain("infantcognitionlab.ucdavis.edu");

	qInfo() << "Number of screens: " << QApplication::desktop()->screenCount();
	for (int i=0; i<QApplication::desktop()->screenCount(); i++)
	{
		qInfo() << "screen " << i << ": screenGeometry: " << QApplication::desktop()->screenGeometry(i);
		qInfo() << "screen " << i << ": availableGeometry: " << QApplication::desktop()->availableGeometry(i);
	}

	if (argc == 2)
	{
		screen = QString(argv[1]).toInt();
		HG3Dialog dialog(QDir("/Users/dan/Desktop/habit22/stim/examples"), screen);
		dialog.exec();
	}
	else if (argc == 3)
	{
		// ignore second arg
		screen = QString(argv[1]).toInt();
		HG3Dialog dialog(QDir("/Users/dan/Desktop/habit22/stim/examples"), screen, true);
		dialog.exec();
	}
	//dialog.show();
	return 0;
}



