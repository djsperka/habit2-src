#include <QApplication>
#include "runsettingsform.h"

void dumpSubjectSettings(Habit::SubjectSettings ss);
void dumpRunSettings(Habit::RunSettings rs);

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	app.setApplicationName("Habit");
	app.setOrganizationName("Infant Cognition Lab");
	app.setOrganizationDomain("infantcognitionlab.ucdavis.edu");

    GUILib::RunSettingsForm runSettingsForm(NULL);
	
	if(runSettingsForm.exec() == QDialog::Accepted) 
	{
		dumpSubjectSettings(runSettingsForm.getSubjectSettings());
		dumpRunSettings(runSettingsForm.getRunSettings());	
	}
	return 0;
}


void dumpSubjectSettings(Habit::SubjectSettings ss)
{
}


void dumpRunSettings(Habit::RunSettings rs)
{
}
