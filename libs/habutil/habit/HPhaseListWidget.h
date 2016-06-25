/*
 * HPhaseListWidget.h
 *
 *  Created on: Jun 24, 2016
 *      Author: dan
 */

#ifndef HPHASELISTWIDGET_H_
#define HPHASELISTWIDGET_H_

#include <QWidget>
#include <QStringList>
#include <QListView>
#include <QToolBar>
#include <QAction>

namespace GUILib
{

	class HPhaseListWidget: public QWidget
	{
		Q_OBJECT

	private:
		QListView *m_pListView;
		QToolBar *m_pPhaseToolBar;
		QAction *m_actionNewPhase;
		QAction *m_actionDelPhase;

		void components();
		void connections();

	public:
		HPhaseListWidget(const QStringList& phaseNames, QWidget* parent=NULL);
		virtual ~HPhaseListWidget();

	private slots:
		void addPhase();
		void delPhase();
	};

};


#endif /* HPHASELISTWIDGET_H_ */
