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
#include <QStringListModel>
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
		QStringListModel *m_pModel;
		QToolBar *m_pPhaseToolBar;
		QAction *m_actionNewPhase;
		QAction *m_actionDelPhase;
		QAction *m_actionUpPhase;
		QAction *m_actionDownPhase;

		void components();
		void connections();

	public:
		HPhaseListWidget(const QStringList& phaseNames, QWidget* parent=NULL);
		virtual ~HPhaseListWidget();
		void clearSelection();
		QString selectedPhase();
		int selectedPhaseIndex();
		int phaseCount();
		void movePhaseDown();
		void movePhaseUp();
		void removePhase(int row);
		void insertPhase(int row, const QString& name);
		QStringList currentPhaseNames() const;
		void setCurrentIndex(int index);

	signals:
		void phaseListViewItemClicked(const QModelIndex&);
		void addPhase();
		void delPhase();
		void upPhase();
		void downPhase();

	public slots:
		void phaseClicked();
		void phaseNameChanged(const QString&);
		void phaseEnabledClicked(bool);
	};

};


#endif /* HPHASELISTWIDGET_H_ */
