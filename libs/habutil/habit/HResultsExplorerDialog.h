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
#include <QFileInfo>
#include <QItemSelection>

namespace Ui
{
	class HResultsExplorerForm;
};



namespace GUILib
{
#if 0
	class HResultsFileSystemModel: public QFileSystemModel
	{
		Q_OBJECT

	public:
		HResultsFileSystemModel(QObject *parent = NULL) : QFileSystemModel(parent) {};
		virtual ~HResultsFileSystemModel() {};
		virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	};
#endif

	class HResultsExplorerDialog: public QDialog
	{
		Q_OBJECT

		Ui::HResultsExplorerForm *ui;
		QDir m_rootDir;
		QFileSystemModel* m_pFolderModel;
		QFileSystemModel* m_pFilesModel;

		void connections();
		void initialize();
		void showResultsFile(const QString filename);
		void openCSVFile(const QFileInfo& info);
	private slots:
//		void folderClicked(QModelIndex index);
		void itemClicked(QModelIndex index);
//		void resultsFileActivated(QModelIndex index);
		void itemActivated(QModelIndex index);
		void openClicked();
		void checkResultsClicked();
		void selectionChanged(const QItemSelection&,const QItemSelection&);

	public:
		HResultsExplorerDialog(const QDir& rootDir, QWidget *parent=NULL);
		virtual ~HResultsExplorerDialog();
		QString getSelectedFile();
	};
};



#endif /* HRESULTSEXPLORERDIALOG_H_ */
