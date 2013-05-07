#include <QApplication>
#include <QMessageBox>
#include "testresults.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	app.setApplicationName("testresults");
	app.setOrganizationName("Infant Cognition Lab");
	app.setOrganizationDomain("infantcognitionlab.ucdavis.edu");

	QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), "~/Desktop", tr("Habit Results Files (*.res)"));
	if (!filename.isNull())
	{
		Habit::ExperimentSettings expSettings;
		Habit::SubjectSettings subjectSettings;
		Habit::RunSettings runSettings;
		HEventLog log;

		if (loadExperimentResults(filename, expSettings, runSettings, subjectSettings, log))
		{
			// Got result. Generate summary.
			QDialog dialog;

		}
		else
		{
			// error opening/reading results
		}
	}

	return 0;
}




