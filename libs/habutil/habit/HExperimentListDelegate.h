/*
 * HExperimentListDelegate.h
 *
 *  Created on: Nov 12, 2014
 *      Author: Oakeslab
 */

#ifndef HEXPERIMENTLISTDELEGATE_H_
#define HEXPERIMENTLISTDELEGATE_H_

#include <QItemDelegate>

class QModelIndex;
class QWidget;
class QVariant;

namespace GUILib
{
	class HExperimentListDelegate : public QItemDelegate
	{
		Q_OBJECT

	public:
		HExperimentListDelegate(QObject *parent = 0);
		QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
		void setEditorData(QWidget *editor, const QModelIndex &index) const;
		void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
		void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
//		void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	};
};


#endif /* HEXPERIMENTLISTDELEGATE_H_ */
