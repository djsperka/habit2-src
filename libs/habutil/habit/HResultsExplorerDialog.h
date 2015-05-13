/*
 * HResultsExplorerDialog.h
 *
 *  Created on: Dec 15, 2014
 *      Author: Oakeslab
 */

#ifndef HRESULTSEXPLORERDIALOG_H_
#define HRESULTSEXPLORERDIALOG_H_

#include <QDialog>
#include <QFileSystemModel>
#include <QDir>
#include "HResultsExplorerFilesProxyModel.h"

namespace Ui
{
	class HResultsExplorerForm;
};

namespace GUILib
{
	class HResultsExplorerDialog: public QDialog
	{
		Q_OBJECT

		Ui::HResultsExplorerForm *ui;
		QDir m_rootDir;
		QFileSystemModel* m_pFolderModel;
		HResultsExplorerFilesProxyModel* m_pFilesProxyModel;
		QFileSystemModel* m_pFilesModel;

		void connections();
		void initialize();
		void showResultsFile(const QString filename);

	private slots:
		void folderClicked(QModelIndex index);
		void resultsFileClicked(QModelIndex index);
		void resultsFileActivated(QModelIndex index);
		void openClicked();

	public:
		HResultsExplorerDialog(const QDir& rootDir, QWidget *parent=NULL);
		virtual ~HResultsExplorerDialog();
		QString getSelectedFile();
	};
};



#endif /* HRESULTSEXPLORERDIALOG_H_ */