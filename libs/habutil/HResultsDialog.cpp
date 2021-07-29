/*
 * HResultsDialog.cpp
 *
 *  Created on: May 28, 2013
 *      Author: Oakeslab
 */

#include "HResultsDialog.h"
#include "HWorkspaceUtil.h"
#include "HExperimentMain.h"
#include <QMessageBox>
#include <QDialog>

#if QT_VERSION >= 0x050000
#include <QPrinter>
#include <QPrintDialog>
#endif

//#include "experimentsettingsform.h"

HResultsDialog::HResultsDialog(const HResults& results, QWidget* parent)
: QDialog(parent)
, m_sExptName(results.experimentSettings().getName())
{
	QVBoxLayout* vlayout = new QVBoxLayout;
	QHBoxLayout* hlayout = new QHBoxLayout;
	hlayout->addWidget(m_pPrintButton = new QPushButton("Print"));
//	hlayout->addWidget(m_pOpenButton = new QPushButton("Open"));
	hlayout->addWidget(m_pExportResultsButton = new QPushButton("Export CSV"));
	hlayout->addWidget(m_pExportLogButton = new QPushButton("Export Event Log"));
	hlayout->addWidget(m_pViewButton = new QPushButton("View Experiment Settings"));
	vlayout->addLayout(hlayout);
	vlayout->addWidget(m_pResultsWidget = new HResultsWidget(results));
	setLayout(vlayout);

	connect(m_pPrintButton, SIGNAL(clicked()), this, SLOT(onPrint()));
	connect(m_pExportLogButton, SIGNAL(clicked()), this, SLOT(onExportLog()));
	connect(m_pViewButton, SIGNAL(clicked()), this, SLOT(onView()));
//	connect(m_pOpenButton, SIGNAL(clicked()), this, SLOT(onOpen()));
	connect(m_pExportResultsButton, SIGNAL(clicked()), this, SLOT(onExportResults()));

}

void HResultsDialog::onPrint()
{
    QPrinter printer;

    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print Document"));
    if (m_pResultsWidget->textCursor().hasSelection())
    	dialog->setOption(QAbstractPrintDialog::PrintSelection);
	if (dialog->exec() != QDialog::Accepted)
        return;

	m_pResultsWidget->print(&printer);

	return;
}

void HResultsDialog::onExportLog()
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

void HResultsDialog::onExportResults()
{
	QFileInfo fileinfo(m_pResultsWidget->results().filename());
	QString tmpfile;
	bool bReplace = false;
	int iResult;

	qDebug() << "Results from Habit version " << m_pResultsWidget->results().version();
	qDebug() << "Current habit version " << QCoreApplication::instance()->applicationVersion();

	// hard-code version 2.1.23 for replacing looks
	if (habutilCompareVersions(QString("2.1.23"), m_pResultsWidget->results().version(), iResult) && iResult == 1)
	{
		QString msg("Results were created by Habit version %1. For results from versions of Habit prior to 2.1.23 we must reanalyze the looking behavior for each trial before export. This is an EXPERIMENTAL FEATURE. Retain the original results file for comparison.");
		int ret = QMessageBox::information(this, tr("Export CSV File"), msg.arg(m_pResultsWidget->results().version()), QMessageBox::Cancel | QMessageBox::Ok, QMessageBox::Ok);
		if (ret == QMessageBox::Cancel) return;
		else
			bReplace = true;
	}

#if 0
	bool b;
	b = habutilCompareVersions(m_pResultsWidget->results().version(), QCoreApplication::instance()->applicationVersion(), result);
	//qDebug() << "compare " << m_pResultsWidget->results().version() << " " << QCoreApplication::instance()->applicationVersion() << " b " << b << " result " << result;

	b = habutilCompareVersions(QString("2.1.29a"), QString("2.1.29b"), result);
	b = habutilCompareVersions(QString("2.1.29a"), QString("2.1.29"), result);
	b = habutilCompareVersions(QString("2.1.29"), QString("2.1.29a"), result);
	b = habutilCompareVersions(QString("2"), QString("2.1.29"), result);
	b = habutilCompareVersions(QString("2.1.29"), QString("2.1.29"), result);
	b = habutilCompareVersions(QString("2.1.29-ABC"), QString("2.1.29-ABC"), result);
	b = habutilCompareVersions(QString("2.1.29-test"), QString("2.1.29"), result);
	b = habutilCompareVersions(QString("2.1.29"), QString("2.1.29-test"), result);
	b = habutilCompareVersions(QString("2.1.29-test"), QString("2.1.29-test"), result);
	b = habutilCompareVersions(QString("2.1.29"), QString("2.2.29"), result);
	b = habutilCompareVersions(QString("2.2.29"), QString("2.1.29"), result);
	b = habutilCompareVersions(QString("3.1.29"), QString("2.1.29"), result);
	b = habutilCompareVersions(QString("2.1.29"), QString("3.1.29"), result);
	b = habutilCompareVersions(QString("2.1.29-A"), QString("2.1.29-B"), result);
	b = habutilCompareVersions(QString("2.1.29-B"), QString("2.1.29-A"), result);
	b = habutilCompareVersions(QString("2.1.29-A"), QString("2.1.29-A1"), result);
	b = habutilCompareVersions(QString("2.1.29-A1"), QString("2.1.29-A"), result);
#endif

	// The filename in the results widget's HResults may not have a directory yet. That's the case when the expt has
	// just been run and we have not saved the results to disk. If a results file has been loaded from disk, though,
	// the filename will have a dir. Attempt to make the save starting point as close as possible to the location
	// of the results (even if the results haven't been written - in that case assume they'll get written to the
	// results dir).

	if (fileinfo.dir().path() == ".")
	{
		tmpfile = habutilGetResultsDir(m_sExptName).path().append("/").append(fileinfo.baseName()).append(".csv");
	}
	else
	{
		tmpfile = fileinfo.dir().path().append("/").append(fileinfo.baseName()).append(".csv");
	}

	// Get filename. Use the original input filename as starting point...
	QString fileName = QFileDialog::getSaveFileName(this, tr("Filename for export"), tmpfile, "CSV File (*.csv)");
	if (!fileName.isNull() && !fileName.isEmpty())
	{
		if (!m_pResultsWidget->results().saveToCSV(fileName, bReplace))
		{
			qCritical() << "Error - cannot save data to file " << fileName;
		}
	}
}
