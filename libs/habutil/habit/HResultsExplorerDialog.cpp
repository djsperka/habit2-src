/*
 * HResultsExplorerDialog.cpp
 *
 *  Created on: Dec 15, 2014
 *      Author: Oakeslab
 */

#include "HResultsExplorerDialog.h"
#include "HResultsDialog.h"
#include "ui_HResultsExplorerForm.h"
#include "HResults.h"
#include "ResultsScannerDialog.h"
#include <QtDebug>
#include <QMessageBox>
#include <QFileInfo>
#include <QDirIterator>

using namespace GUILib;

HResultsExplorerDialog::HResultsExplorerDialog(const QDir& rootDir, QWidget *parent)
: QDialog(parent)
, ui(new Ui::HResultsExplorerForm)
, m_rootDir(rootDir)
{
	ui->setupUi(this);
	initialize();
	connections();
}

HResultsExplorerDialog::~HResultsExplorerDialog()
{
	delete ui;
}


void HResultsExplorerDialog::initialize()
{
	setWindowTitle("Habit Results Explorer");
	m_pFolderModel = new QFileSystemModel(this);
//	m_pFolderModel = new HResultsExplorerDialogFolderModel(this);
	m_pFolderModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);
	ui->treeViewFolders->setModel(m_pFolderModel);
	ui->treeViewFolders->setRootIndex(m_pFolderModel->setRootPath(m_rootDir.absolutePath()));

	// select entire rows
	ui->treeViewFolders->setSelectionBehavior(QAbstractItemView::SelectRows);

	// hide all columns except for first, and hide the header itself
	ui->treeViewFolders->header()->hide();
	for (int i=1; i<4; i++)
		ui->treeViewFolders->hideColumn(i);

	QStringList nameFilter;
	nameFilter << "*.hab" << "*.csv";
	m_pFolderModel->setNameFilters(nameFilter);
	m_pFolderModel->setNameFilterDisables(false);

	// Open pushbutton initially disabled - it is enabled when something is clicked.
	ui->pbOpen->setEnabled(false);

	// Check pushbutton initially disabled
	ui->pbCheckResults->setEnabled(false);
}

void HResultsExplorerDialog::connections()
{
	connect(ui->pbCancel, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui->pbOpen, SIGNAL(clicked()), this, SLOT(openClicked()));
	connect(ui->pbCheckResults, SIGNAL(clicked()), this, SLOT(checkResultsClicked()));
	connect(ui->treeViewFolders, SIGNAL(activated(QModelIndex)), this, SLOT(itemActivated(QModelIndex)));
	connect(ui->treeViewFolders->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)), this, SLOT(selectionChanged(const QItemSelection&,const QItemSelection&)));
}

void HResultsExplorerDialog::selectionChanged(const QItemSelection& selected,const QItemSelection&)
{
	QModelIndexList indexes = selected.indexes();
	if (indexes.size() > 0)
	{
		if (m_pFolderModel->fileInfo(indexes.first()).isFile())
		{
			ui->pbOpen->setEnabled(true);
			// only enable check button if its a results file
			if (m_pFolderModel->fileInfo(indexes.first()).suffix().contains("hab", Qt::CaseInsensitive))
			{
				ui->pbCheckResults->setEnabled(true);
			}
			else
			{
				ui->pbCheckResults->setEnabled(false);
			}
		}
		else
		{
			ui->pbOpen->setEnabled(false);
			ui->pbCheckResults->setEnabled(true);
		}
	}
	else
	{
		ui->pbOpen->setEnabled(false);
		ui->pbCheckResults->setEnabled(false);
	}

}

//void HResultsExplorerDialog::resultsFileClicked(QModelIndex index)
//{
//	Q_UNUSED(index);
//	//QString sPath = m_pFilesModel->fileInfo(m_pFilesProxyModel->mapFromSource(index)).absoluteFilePath();
//	//qDebug() << "got file " << sPath;
//	ui->pbOpen->setEnabled(true);
//}
//
//void HResultsExplorerDialog::resultsFileActivated(QModelIndex index)
//{
//	Q_UNUSED(index);
//	showResultsFile(getSelectedFile());
//}

void HResultsExplorerDialog::itemActivated(QModelIndex index)
{
	// ignore dirs
	if (m_pFolderModel->isDir(index))
	{
		return;
	}
	else
	{
		QFileInfo info = m_pFolderModel->fileInfo(index);
		QString suffix = info.completeSuffix().toLower();
		if (suffix == "hab")
		{
			qDebug() << "hab file activated: filePath " << m_pFolderModel->filePath(index);
			showResultsFile(info.absoluteFilePath());
		}
		else if (suffix == "csv")
		{
			qDebug() << "csv file activated: filePath " << m_pFolderModel->filePath(index);
			openCSVFile(info);
		}
	}
}

void HResultsExplorerDialog::openCSVFile(const QFileInfo& info)
{
	#ifdef Q_OS_MACOS
	QStringList args;
	args << "-e";
	args << "tell application \"Microsoft Excel\"";
	args << "-e";
	args << "activate";
	args << "-e";
	args << "open \"" + info.canonicalFilePath() +"\"";
//	args << "-e";
//	args << "set myfile to POSIX file \"" + info.canonicalFilePath() + "\"";
//	args << "-e";
//	args << "open myfile";
	args << "-e";
	args << "end tell";
	QProcess::startDetached("osascript", args);
	#endif

	#ifdef Q_OS_WIN
	QStringList args;
	args << "/select," << QDir::toNativeSeparators(info.canonicalFilePath());
	QProcess::startDetached("explorer", args);
	#endif
}


void HResultsExplorerDialog::openClicked()
{
	// maybe we should not assume that open cannot be clicked until something is selected.
	// not sure how that would happen, but make sure.
	if (ui->treeViewFolders->selectionModel()->hasSelection())
		showResultsFile(getSelectedFile());
	accept();
}

void HResultsExplorerDialog::checkResultsClicked()
{
#if 0
	QString sHtml;
	QTextStream output(&sHtml);
	QFileInfo selectedFile(getSelectedFile());

	// style for the text edit
	output << "<style>";
	output << " .okres { background-color: #90E090; margin-left: 10px;}";
	output << " .warnres { background-color: #F0F090; margin-left: 10px;}";
	output << " .errres { background-color: #F09090; margin-left: 10px;}";
	output << " .results { margin-left: 20px; color:#808080; font-size:75%;}";
	//output << " .okres .results { background-color: #90E090; margin-left: 20px; color:#808080; font-size:75%;}";
	//output << " .warnres .results { background-color: #F0F090; margin-left: 20px; color:#808080; font-size:75%;}";
	//output << " .errres .results { background-color: #F09090; margin-left: 20px; color:#808080; font-size:75%;}";
	output << "</style>";

	if (selectedFile.isDir())
	{
		// heading line for output
		output << "Scanning results in folder: <b>" << getSelectedFile() << "</b><br>";
		QDirIterator it(getSelectedFile(), QStringList() << "*.hab", QDir::Files, QDirIterator::NoIteratorFlags);
		while (it.hasNext())
		{
		    QFileInfo inf(it.next());
		    QString sFileScanResults;
		    QTextStream outputFileScanResults(&sFileScanResults);

			// scan file and get count of number of duplicate looks.
			// Begin html output once results are in.
		    int i = HResults::checkHabFileForDups(it.next(), outputFileScanResults);
			if (i > 0)
			{
				outputFileScanResults << QString("Found %1 duplicate looks.<br>").arg(i);
				output <<  "<div class='warnres'>";
			}
			else if (i == 0)
			{
				outputFileScanResults << "No duplicate looks found.<br>";
				output <<  "<div class='okres'>";
			}
			else
			{
				outputFileScanResults << "Cannot read file.<br>";
				output <<  "<div class='errres'>";
			}
			output <<  "<p>";
		    output << "Results file: <i>" << inf.completeBaseName() << "</i>";
		    // results from the scanner
		    output << "<div class='results'>";
			output << sFileScanResults;
		    output << "</div>";
		    output << "</p>";
		    output << "</div>";
		}
	}
	else
	{
		output << "Results file: " << getSelectedFile() << Qt::endl;
		int i = HResults::checkHabFileForDups(getSelectedFile(), output);
		if (i > -1)
			output << QString("Found %1 duplicate looks.").arg(i) << Qt::endl;
		else
			output << "Cannot open file." << Qt::endl;
		qDebug().noquote() << sHtml;
	}
#else
	QString sHtml;
	QTextStream output(&sHtml);
	QFileInfo selectedFile(getSelectedFile());

	auto styleHtml = [&output]()
		{
			output << "<style>"
					<< " .okres { background-color: #90E090; margin-left: 10px;}"
					<< " .warnres { background-color: #F0F090; margin-left: 10px;}"
					<< " .errres { background-color: #F09090; margin-left: 10px;}"
					<< " .results { margin-left: 20px; color:#808080; font-size:50%;}"
					<< "</style>";
		};
	auto singleFileHtml = [&output] (const QString& filename, bool isLi)
	{
	    QFileInfo inf(filename);
	    QString sFileScanResults;
	    QString sClass;
	    QTextStream outputFileScanResults(&sFileScanResults);

		// scan file and get count of number of duplicate looks.
		// Begin html output once results are in.
	    int i = HResults::checkHabFileForDups(filename, outputFileScanResults);
		if (i > 0)
		{
			outputFileScanResults << QString("Found %1 duplicate looks.<br>").arg(i);
			sClass = "warnres";
			//output <<  "<div class='warnres'>";
		}
		else if (i == 0)
		{
			outputFileScanResults << "No duplicate looks found.<br>";
			sClass = "okres";
			//output <<  "<div class='okres'>";
		}
		else
		{
			outputFileScanResults << "Cannot read file.<br>";
			sClass = "errres";
			//output <<  "<div class='errres'>";
		}

		// list element
		if (isLi)
		{
			output << QString("<li class='%1'><i>%2</i></li>").arg(sClass).arg(inf.completeBaseName());

			// results from the scanner
			output << "<div class='results'>"
				<< sFileScanResults
				<< "</div>";
		}
		else
		{
			output << QString("<p class='%1'><i>%2</i></p>").arg(sClass).arg(inf.completeBaseName());

			// results from the scanner
			output << "<div class='results'>"
				<< sFileScanResults
				<< "</div>";
		}
	};

	if (selectedFile.isDir())
	{
		// style element
		styleHtml();

		// heading line for output, list element
		output << "Scanning experimental results in folder: <br><b>" << getSelectedFile() << "</b><br>";
		output << "<ul style=\"list-style-type:none;\">";
		QDirIterator it(getSelectedFile(), QStringList() << "*.hab", QDir::Files, QDirIterator::NoIteratorFlags);
		while (it.hasNext())
		{
			singleFileHtml(it.next(), true);
		}
		output << "</ul>";
	}
	else
	{
		styleHtml();
		singleFileHtml(getSelectedFile(), false);
	}

#endif
	qDebug() << sHtml;

	ResultsScannerDialog rsd(this, sHtml);
	rsd.show();
	rsd.exec();
}


QString HResultsExplorerDialog::getSelectedFile()
{
	QModelIndexList selectedIndexes = ui->treeViewFolders->selectionModel()->selectedIndexes();
	return m_pFolderModel->fileInfo(selectedIndexes.at(0)).absoluteFilePath();
}

void HResultsExplorerDialog::showResultsFile(const QString filename)
{
	HResults* results = HResults::load(filename);
	if (!results)
	{
		QMessageBox msgBox;
		msgBox.setText("Cannot load data.");
		msgBox.exec();
	}
	else
	{
		HResultsDialog dialog(*results, NULL);
		dialog.exec();
		delete results;
	}
	return;
}

