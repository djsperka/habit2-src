#include "resultviewerform.h"
#include <QtGui/QFileDialog>
#include <QtCore/QTextStream>
#include <QtGui/QPrintDialog>

ResultViewerForm::ResultViewerForm(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

ResultViewerForm::~ResultViewerForm()
{

}

void ResultViewerForm::setLogText(const QStringList &logText)
{
	ui.logBrowser->setPlainText("");
	for (int i= 0; i<logText.count(); i++)
	ui.logBrowser->append(logText[i]);
}

void ResultViewerForm::onSavePressed()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Experiment Result"),
								"", tr("Experiment Log File (*.txt)"));
	if (!fileName.isNull() && !fileName.isEmpty())
	{
		QFile file(fileName);
		if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
		{
			QTextStream out(&file);
			out << ui.logBrowser->toPlainText();
		}
		file.close();
	}
}

void ResultViewerForm::onPrintPressed()
{
	QPrintDialog dlg(this);
	if (dlg.exec() == QDialog::Accepted)
	{
		ui.logBrowser->print(dlg.printer());
	}
}