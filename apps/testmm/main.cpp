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

	app.setApplicationName("habit2");
	app.setOrganizationName("Infant Cognition Lab");
	app.setOrganizationDomain("infantcognitionlab.ucdavis.edu");


	// Default is to always have log file.
	for (int i=1; i<argc; i++)
	{
		if (!strcmp(argv[i], "-x"))
		{
			habutilClearWorkspace();
		}
		else if (!strcmp(argv[i], "-w"))
		{
			// TODO - maybe tell user they need to supply a directory?
			if (i < (argc-1))
			{
				habutilSetWorkspace(argv[i+1]);
				i++;
			}
		}
		else if (!strcmp(argv[i], "-r"))
		{
			habutilSetStimulusRootDir(QString(argv[i+1]));
			i++;
		}
		else
		{
			qDebug() << "Unrecognized command line arg " << QString(argv[i]);
		}
	}


	if (!habutilInitWorkspace())
		return 0;

	// Select an experiment to test with
	HExperimentSelectionDialog *dialog = new HExperimentSelectionDialog();

	dialog->show();


	return app.exec();
}
