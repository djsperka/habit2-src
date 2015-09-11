/*
 * main.cpp
 *
 *  Created on: Jan 27, 2014
 *      Author: Oakeslab
 */

#include "TestKPDialog.h"
#include <QApplication>

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	TestKPDialog dialog;
	dialog.show();

	return app.exec();
}
