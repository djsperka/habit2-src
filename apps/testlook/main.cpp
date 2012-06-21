#include "HLooker.h"
#include "TestLookDialog.h"
#include <QApplication>


int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	TestLookDialog *dlg = new TestLookDialog();

//	HLooker h(1000, 250);

	dlg->show();
	return app.exec();			
	return 0;
}