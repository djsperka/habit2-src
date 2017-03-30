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
#include "HAudioPlayer5.h"
#include "Q5PlayerTestDialog.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	app.setApplicationName("q5player");
	app.setOrganizationName("Infant Cognition Lab");
	app.setOrganizationDomain("infantcognitionlab.ucdavis.edu");

	Q5PlayerTestDialog dialog(QDir("/Users/dan/Desktop/habit22/stim/examples"));
	dialog.exec();
//	QMessageBox msgBox;
//	msgBox.setText("The document has been modified.");
//	msgBox.exec();

	return 0;
}



