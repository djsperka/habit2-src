/*
 * q5player.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: dan
 */

#include <QApplication>
#include "FDDialog.h"

//#include "HAudioPlayer5.h"
#include "FDDialog.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	FDDialog *pdlg = new FDDialog();
	pdlg->exec();
	delete pdlg;
	return 0;
}



