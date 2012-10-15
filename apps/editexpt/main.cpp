#include <QApplication>
#include "EditExptDialog.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	app.setApplicationName("Habit");
	app.setOrganizationName("Infant Cognition Lab");
	app.setOrganizationDomain("infantcognitionlab.ucdavis.edu");
	
	EditExptDialog* d = new EditExptDialog;
	d->exec();

	return 0;
}

