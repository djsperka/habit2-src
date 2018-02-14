/*
 *  main.cpp
 *  myp
 *
 *  Created by Oakes Lab on 5/17/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

//#include "stimulusdisplayinfo.h"
//#include "HExperimentSelectionDialog.h"
#include "HExperimentListWidget.h"
#include "HWorkspaceUtil.h"
#include <QApplication>
#include <QtDebug>
#include <gst/gst.h>


#include "TestMMDialog.h"

int main(int argc, char **argv)
{
	QString script;
	QApplication app(argc, argv);
	gst_init(&argc, &argv);
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
	//HExperimentSelectionDialog *dialog = new HExperimentSelectionDialog();
	//HExperimentListWidget *w = new HExperimentListWidget(0, true, true);
	//w->show();
	//dialog->show();

	TestMMDialog *dialog = new TestMMDialog();
	dialog->exec();

	return 0;
}
