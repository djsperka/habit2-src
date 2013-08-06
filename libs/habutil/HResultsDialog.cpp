/*
 * HResultsDialog.cpp
 *
 *  Created on: May 28, 2013
 *      Author: Oakeslab
 */

#include "HResultsDialog.h"
#include <QPrinter>
#include <QPrintDialog>

HResultsDialog::HResultsDialog(const HResults& results, QWidget* parent)
: QDialog(parent)
{
	QVBoxLayout* vlayout = new QVBoxLayout;
	QHBoxLayout* hlayout = new QHBoxLayout;
	hlayout->addWidget(m_pPrintButton = new QPushButton("Print"));
	hlayout->addWidget(m_pExportButton = new QPushButton("Export CSV"));
	vlayout->addLayout(hlayout);
	vlayout->addWidget(m_pResultsWidget = new HResultsWidget(results));
	setLayout(vlayout);

	connect(m_pPrintButton, SIGNAL(clicked()), this, SLOT(onPrint()));
	connect(m_pExportButton, SIGNAL(clicked()), this, SLOT(onExport()));
}

void HResultsDialog::onPrint()
{
    QPrinter printer;

    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print Document"));
    if (m_pResultsWidget->textCursor().hasSelection())
    	dialog->addEnabledOption(QAbstractPrintDialog::PrintSelection);
	if (dialog->exec() != QDialog::Accepted)
        return;

	m_pResultsWidget->print(&printer);

	return;
}

void HResultsDialog::onExport()
{
	QFileInfo fileinfo(m_pResultsWidget->results().filename());
	QString tmpfile = fileinfo.dir().path().append("/").append(fileinfo.baseName()).append(".csv");

	// Get filename. Use the original input filename as starting point...
	QString fileName = QFileDialog::getSaveFileName(this, tr("Filename for export"), tmpfile, "CSV File (*.csv)");
	if (!fileName.isNull() && !fileName.isEmpty())
	{
		if (!m_pResultsWidget->results().eventLog().saveToCSV(fileName))
		{
			qCritical() << "Error - cannot save data to file " << fileName;
		}
	}
}

