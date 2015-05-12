/*
 * HResultsExplorerFilesProxyModel.h
 *
 *  Created on: Dec 15, 2014
 *      Author: Oakeslab
 */

#ifndef HRESULTSEXPLORERFILESPROXYMODEL_H_
#define HRESULTSEXPLORERFILESPROXYMODEL_H_

#include <QFileSystemModel>
#include <QIdentityProxyModel>

namespace GUILib
{
	class HResultsExplorerFilesProxyModel: public QIdentityProxyModel
	{
		Q_OBJECT

		QFileSystemModel* m_pFilesModel;

	public:
		HResultsExplorerFilesProxyModel(QFileSystemModel* pFilesModel, QObject *parent = NULL) : QIdentityProxyModel(parent), m_pFilesModel(pFilesModel) { setSourceModel(pFilesModel); };
		virtual ~HResultsExplorerFilesProxyModel() {};

		// overrides
		int columnCount(const QModelIndex& parent = QModelIndex()) const;
		QVariant data(const QModelIndex &index, int role) const;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	};
};


#endif /* HRESULTSEXPLORERFOLDERMODEL_H_ */
