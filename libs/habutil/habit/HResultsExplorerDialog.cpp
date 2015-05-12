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
#include <QtDebug>
#include <QMessageBox>
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
	m_pFolderModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
	ui->treeViewFolders->setModel(m_pFolderModel);
	ui->treeViewFolders->setRootIndex(m_pFolderModel->setRootPath(m_rootDir.absolutePath()));

	// hide all columns except for first, and hide the header itself
	ui->treeViewFolders->header()->hide();
	for (int i=1; i<4; i++)
		ui->treeViewFolders->hideColumn(i);

	QStringList nameFilter;
	nameFilter << "*.hab";

	m_pFilesModel = new QFileSystemModel(this);
	m_pFilesModel->setRootPath(m_rootDir.absolutePath());
	m_pFilesModel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
	m_pFilesModel->setNameFilters(nameFilter);
	m_pFilesModel->setNameFilterDisables(false);

	m_pFilesProxyModel = new HResultsExplorerFilesProxyModel(m_pFilesModel, this);
	// done in constructor m_pFilesProxyModel->setSourceModel(m_pFilesModel);

	ui->tableViewFiles->setModel(m_pFilesProxyModel);
	ui->tableViewFiles->setRootIndex(m_pFilesProxyModel->mapFromSource(m_pFilesModel->setRootPath(m_rootDir.absolutePath())));
	ui->tableViewFiles->setShowGrid(false);
	ui->tableViewFiles->verticalHeader()->hide();
	ui->tableViewFiles->horizontalHeader()->setStretchLastSection(true);
	ui->tableViewFiles->setSelectionBehavior(QAbstractItemView::SelectRows);

	// Open pushbutton initially disabled - it is enabled when something is clicked.
	ui->pbOpen->setEnabled(false);
}

void HResultsExplorerDialog::connections()
{
	connect(ui->pbCancel, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui->pbOpen, SIGNAL(clicked()), this, SLOT(openClicked()));
	connect(ui->treeViewFolders, SIGNAL(clicked(QModelIndex)), this, SLOT(folderClicked(QModelIndex)));
	connect(ui->tableViewFiles, SIGNAL(clicked(QModelIndex)), this, SLOT(resultsFileClicked(QModelIndex)));
	connect(ui->tableViewFiles, SIGNAL(activated(QModelIndex)), this, SLOT(resultsFileActivated(QModelIndex)));
}

void HResultsExplorerDialog::folderClicked(QModelIndex index)
{
	QString sPath = m_pFolderModel->fileInfo(index).absoluteFilePath();
	ui->tableViewFiles->setRootIndex(m_pFilesProxyModel->mapFromSource(m_pFilesModel->setRootPath(sPath)));
}

void HResultsExplorerDialog::resultsFileClicked(QModelIndex index)
{
	Q_UNUSED(index);
	//QString sPath = m_pFilesModel->fileInfo(m_pFilesProxyModel->mapFromSource(index)).absoluteFilePath();
	//qDebug() << "got file " << sPath;
	ui->pbOpen->setEnabled(true);
}

void HResultsExplorerDialog::resultsFileActivated(QModelIndex index)
{
	Q_UNUSED(index);
	showResultsFile(getSelectedFile());
}

void HResultsExplorerDialog::openClicked()
{
	// maybe we should not assume that open cannot be clicked until something is selected.
	// not sure how that would happen, but make sure.
	if (ui->tableViewFiles->selectionModel()->hasSelection())
		showResultsFile(getSelectedFile());
	accept();
}

QString HResultsExplorerDialog::getSelectedFile()
{
	QModelIndexList selectedIndexes = ui->tableViewFiles->selectionModel()->selectedIndexes();
	return m_pFilesModel->fileInfo(m_pFilesProxyModel->mapFromSource(selectedIndexes.at(0))).absoluteFilePath();
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
