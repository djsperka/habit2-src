#include "HLooker.h"
#include "TestTrialDialog.h"
#include <QApplication>
#include <QtGui>

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	TestTrialDialog *dlg = new TestTrialDialog();
	dlg->show();
	return app.exec();			
	return 0;
}