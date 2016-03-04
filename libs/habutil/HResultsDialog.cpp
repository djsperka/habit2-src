/*
 * HResultsDialog.cpp
 *
 *  Created on: May 28, 2013
 *      Author: Oakeslab
 */

#include "HResultsDialog.h"
#include "HWorkspaceUtil.h"
#include "HExperimentMain.h"
//#include "experimentsettingsform.h"

HResultsDialog::HResultsDialog(const HResults& results, QWidget* parent)
: QDialog(parent)
, m_sExptName(results.experimentSettings().getName())
{
	QVBoxLayout* vlayout = new QVBoxLayout;
	QHBoxLayout* hlayout = new QHBoxLayout;
	hlayout->addWidget(m_pPrintButton = new QPushButton("Print"));
	hlayout->addWidget(m_pOpenButton = new QPushButton("Open"));
	hlayout->addWidget(m_pGenerateButton = new QPushButton("(Re)Generate Results"));
	hlayout->addWidget(m_pExportButton = new QPushButton("Export Event Log"));
	hlayout->addWidget(m_pViewButton = new QPushButton("View Experiment Settings"));
	vlayout->addLayout(hlayout);
	qDebug() << "HResultsDialog::HResultsDialog(const HResults& results, QWidget* parent) - new HResultsWidget";
	vlayout->addWidget(m_pResultsWidget = new HResultsWidget(results));
	qDebug() << "HResultsDialog::HResultsDialog(const HResults& results, QWidget* parent) - new HResultsWidget done";
	setLayout(vlayout);

	connect(m_pPrintButton, SIGNAL(clicked()), this, SLOT(onPrint()));
	connect(m_pExportButton, SIGNAL(clicked()), this, SLOT(onExport()));
	connect(m_pViewButton, SIGNAL(clicked()), this, SLOT(onView()));
	connect(m_pOpenButton, SIGNAL(clicked()), this, SLOT(onOpen()));
	connect(m_pGenerateButton, SIGNAL(clicked()), this, SLOT(onGenerate()));
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
	QString tmpfile;

	// The filename in the results widget's HResults may not have a directory yet. That's the case when the expt has
	// just been run and we have not saved the results to disk. If a results file has been loaded from disk, though,
	// the filename will have a dir. Attempt to make the save starting point as close as possible to the location
	// of the results (even if the results haven't been written - in that case assume they'll get written to the
	// results dir).

	if (fileinfo.dir().path() == ".")
	{
		tmpfile = habutilGetResultsDir(m_sExptName).path().append("/").append(fileinfo.baseName()).append("_eventlog.csv");
	}
	else
	{
		tmpfile = fileinfo.dir().path().append("/").append(fileinfo.baseName()).append("_eventlog.csv");
	}

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

void HResultsDialog::onView()
{
    GUILib::HExperimentMain experimentMain(m_pResultsWidget->results().experimentSettings(), this, true);
    experimentMain.exec();
}

void HResultsDialog::onOpen()
{
	// Determine CSV filename
	QFileInfo fileinfo(m_pResultsWidget->results().filename());
	QFileInfo csvfile(fileinfo.dir(), fileinfo.completeBaseName() + ".csv");

	// Check if the file exists
	if (!csvfile.exists())
	{
		QMessageBox mbox;
		mbox.setText("Cannot find results file " + csvfile.canonicalFilePath());
		mbox.exec();
	}
	else
	{
		// Open (assuming that Excel is installed!)
#ifdef Q_WS_MAC
	    QStringList args;
	    args << "-e";
	    args << "tell application \"Microsoft Excel\"";
	    args << "-e";
	    args << "activate";
	    args << "-e";
	    args << "open \"" + csvfile.canonicalFilePath() +"\"";
	    args << "-e";
	    args << "end tell";
	    QProcess::startDetached("osascript", args);
#endif

#ifdef Q_WS_WIN
	    QStringList args;
	    args << "/select," << QDir::toNativeSeparators(filePath);
	    QProcess::startDetached("explorer", args);
#endif
	}

}

void HResultsDialog::onGenerate()
{
	QMessageBox mbox;
	mbox.setText("Generate results");
	mbox.exec();
}
