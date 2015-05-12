/*
 * HResultsExplorerFilesProxyModel.cpp
 *
 *  Created on: Dec 19, 2014
 *      Author: Oakeslab
 */

#include "HResultsExplorerFilesProxyModel.h"
#include <QtDebug>
#include <QFileInfo>
using namespace GUILib;

QVariant HResultsExplorerFilesProxyModel::data(const QModelIndex &index, int role) const
{
	QVariant result;

	if (role == Qt::DisplayRole)
	{
		// Get and parse filename
		QString r("???");
		QString filename = m_pFilesModel->fileInfo(this->mapFromSource(index)).baseName();
		QString allowedName("([a-zA-Z0-9-]([a-zA-Z0-9- ]*[a-zA-Z0-9-])?)");
		QString underscore("_");
		QString datetime("([0-9]{4}-[0-9]{2}-[0-9]{2}-[0-9]{4})");
		QRegExp re(allowedName + underscore + allowedName + underscore + datetime);
		if (re.exactMatch(filename))
		{
	#if 0
			qDebug() << "got match input " << filename;
			qDebug() << "0 " << re.cap(0);
			qDebug() << "1 " << re.cap(1);
			//qDebug() << "2 " << re.cap(2);
			qDebug() << "3 " << re.cap(3);
			//qDebug() << "4 " << re.cap(4);
			qDebug() << "5 " << re.cap(5);
	#endif
			if (index.column() == 0)
			{
				r = re.cap(3);
			}
			else if (index.column() == 1)
			{
				r = re.cap(5);
			}
		}
		result = QVariant(r);
	}
	else if (role == Qt::TextAlignmentRole)
	{
		result = QVariant(Qt::AlignCenter);
	}
	else if (role == Qt::DecorationRole)
	{
		// dummy placeholder to indicate we are ignoring decoration role. This prevents the habit icon
		// from being displayed for the file.
	}
	else
	{
		result = QIdentityProxyModel::data(index, role);
	}
	return result;
}


int HResultsExplorerFilesProxyModel::columnCount(const QModelIndex& parent) const
{
	return 2;
}

QVariant HResultsExplorerFilesProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QIdentityProxyModel::headerData(section, orientation, role);

	switch(section)
	{
	case 0: return QVariant(QString("Subject"));
	case 1: return QVariant(QString("Date"));
	}
	return QIdentityProxyModel::headerData(section, orientation, role);
}
