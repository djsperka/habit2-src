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
	QString s;
	QTextStream output(&s);
	QFileInfo selectedFile(getSelectedFile());
	if (selectedFile.isDir())
	{
		output << "Checking dir: " << getSelectedFile();
		// Scan folder for individual files
	}
	else
	{
		output << "Checking file: " << getSelectedFile() << Qt::endl;
		int i = HResults::checkHabFileForDups(getSelectedFile(), output);
		if (i > -1)
			output << QString("Found %1 duplicate looks.").arg(i) << Qt::endl;
		else
			output << "Cannot open file." << Qt::endl;
		qDebug().noquote() << s;
	}

	ResultsScannerDialog rsd(this, s);
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

