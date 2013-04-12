#include "cloneexperimentdialog.h"
#include "maindao.h"

namespace GUILib {

CloneExperimentDialog::CloneExperimentDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	Habit::MainDao dao;
	QStringList sl = dao.getAllExperimentNames();
	if(!sl.isEmpty()) 
	{
		ui.experimentCmbBox->addItems(sl);
	}
	ui.experimentCmbBox->setCurrentIndex(0);
}

CloneExperimentDialog::~CloneExperimentDialog()
{

}

QString CloneExperimentDialog::getExperimentName()
{
	return ui.experimentCmbBox->currentText();
}

}