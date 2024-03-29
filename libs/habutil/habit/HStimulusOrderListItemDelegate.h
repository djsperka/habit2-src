/*
 * HStimulusOrderListItemDelegate.h
 *
 *  Created on: Apr 20, 2015
 *      Author: Oakeslab
 */

#ifndef HSTIMULUSORDERLISTITEMDELEGATE_H_
#define HSTIMULUSORDERLISTITEMDELEGATE_H_

#include <QStyledItemDelegate>

namespace GUILib
{
	class HStimulusOrderListItemDelegate : public QStyledItemDelegate
	{
		Q_OBJECT

	public:
		HStimulusOrderListItemDelegate(QWidget *parent = 0) : QStyledItemDelegate(parent) {}

//		void paint(QPainter *painter, const QStyleOptionViewItem &option,
//				   const QModelIndex &index) const;
//		QSize sizeHint(const QStyleOptionViewItem &option,
//					   const QModelIndex &index) const;
		QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
							  const QModelIndex &index) const;
		void setEditorData(QWidget *editor, const QModelIndex &index) const;
		void setModelData(QWidget *editor, QAbstractItemModel *model,
						  const QModelIndex &index) const;

//	private slots:
//		void commitAndCloseEditor();
	};
};



#endif /* HSTIMULUSORDERLISTITEMDELEGATE_H_ */
