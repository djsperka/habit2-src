/*
 *  main.cpp
 *  myp
 *
 *  Created by Oakes Lab on 5/17/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "stimulusdisplayinfo.h"
#include "SDIDialog.h"
#include <QApplication>

using namespace Habit;

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	StimulusDisplayInfo sdi;

	SDIDialog dialog;
	dialog.show();

	return app.exec();
}
