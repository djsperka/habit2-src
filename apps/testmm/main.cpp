/*
 *  main.cpp
 *  myp
 *
 *  Created by Oakes Lab on 5/17/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "stimulusdisplayinfo.h"
#include "HExperimentSelectionDialog.h"
#include "HWorkspaceUtil.h"
#include <QApplication>

using namespace Habit;

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	//app.setApplicationVersion();
	app.setApplicationName("testmm");
	app.setOrganizationName("Infant Cognition Lab");
	app.setOrganizationDomain("infantcognitionlab.ucdavis.edu");

	if (!habutilInitWorkspace())
		return 0;

	// Select an experiment to test with
	HExperimentSelectionDialog dialog;
	dialog.show();


	return app.exec();
}
