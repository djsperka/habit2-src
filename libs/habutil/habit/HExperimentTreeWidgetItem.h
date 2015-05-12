/*
 * HExperimentTreeWidgetItem.h
 *
 *  Created on: Sep 17, 2014
 *      Author: Oakeslab
 */

#ifndef HEXPERIMENTTREEWIDGETITEM_H_
#define HEXPERIMENTTREEWIDGETITEM_H_

#include <QTreeWidgetItem>
#include <iostream>

namespace GUILib
{
	class HExperimentTreeWidgetItem: public QTreeWidgetItem
	{
	//	Q_OBJECT

	public:
		HExperimentTreeWidgetItem(QTreeWidget* parent, int stackid, const QString& text): QTreeWidgetItem(parent, QTreeWidgetItem::UserType), m_stackid(stackid) { setText(0, text); };
		HExperimentTreeWidgetItem(QTreeWidgetItem* parent, int stackid, const QString& text): QTreeWidgetItem(parent, QTreeWidgetItem::UserType), m_stackid(stackid) { setText(0, text); };
		int stackid() { return m_stackid; };
	private:
		int m_stackid;

	};
};


#endif /* HEXPERIMENTTREEWIDGETITEM_H_ */
