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
		HExperimentTreeWidgetItem(QTreeWidget* parent, int stackid, const QString& text, bool isPhase=false): QTreeWidgetItem(parent, QTreeWidgetItem::UserType), m_stackid(stackid), m_isPhase(isPhase) { setText(0, text); };
		HExperimentTreeWidgetItem(QTreeWidgetItem* parent, int stackid, const QString& text, bool isPhase=false): QTreeWidgetItem(parent, QTreeWidgetItem::UserType), m_stackid(stackid), m_isPhase(isPhase) { setText(0, text); };
		int stackid() { return m_stackid; };
		bool isPhase() { return m_isPhase; };
	private:
		int m_stackid;
		bool m_isPhase;

	};
};


#endif /* HEXPERIMENTTREEWIDGETITEM_H_ */
