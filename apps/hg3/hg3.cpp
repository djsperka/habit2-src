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
#include <gst/gst.h>


//#include "HAudioPlayer5.h"
#include "HG3Dialog.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	gst_init(&argc, &argv);
	int screen = -1;
	QString flag;

	app.setApplicationName("hg3");
	app.setOrganizationName("Infant Cognition Lab");
	app.setOrganizationDomain("infantcognitionlab.ucdavis.edu");

	if (argc >= 2)
	{
		screen = QString(argv[1]).toInt();
	}
	if (argc >= 3)
	{
		flag = argv[2];
	}

	while (QMessageBox::Yes == QMessageBox::question(NULL, "Continue?", "Continue?"))
	{

		HG3Dialog dialog(QDir("/Users/dan/Desktop/habit22/stim/examples"), screen, flag);
		dialog.exec();
	}
	//dialog.show();
	return 0;
}



