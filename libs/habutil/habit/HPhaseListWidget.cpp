/*
 * HPhaseListWidget.cpp
 *
 *  Created on: Jun 24, 2016
 *      Author: dan
 */

#include "HPhaseListWidget.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include <QBrush>

using namespace GUILib;

GUILib::HPhaseListWidget::HPhaseListWidget(const QStringList& phaseNames, QWidget* parent)
: QWidget(parent)
{
	components();
	m_pModel = new QStringListModel(phaseNames);
	m_pListView->setModel(m_pModel);
	connections();
}

GUILib::HPhaseListWidget::~HPhaseListWidget()
{
}

void  GUILib::HPhaseListWidget::components()
{
	m_pListView = new QListView(this);
	m_pListView->setEditTriggers(QAbstractItemView::NoEditTriggers);		// cannot change name of phase directly in listview - use HPhaseSettingsTabWidget

	m_pPhaseToolBar = new QToolBar();
	m_actionNewPhase = new QAction(QIcon(":/resources/expt-main/p-add.png"), "Add new phase", this);
	m_actionDelPhase = new QAction(QIcon(":/resources/expt-main/p-delete.png"), "Delete selected phase", this);
	m_actionUpPhase = new QAction(QIcon(":/resources/expt-main/p-up.png"), "Move selected phase up", this);
	m_actionDownPhase = new QAction(QIcon(":/resources/expt-main/p-down.png"), "Move selected phase down", this);
	m_pPhaseToolBar->addAction(m_actionNewPhase);
	connect(m_actionNewPhase, SIGNAL(triggered()), this, SIGNAL(addPhase()));
	m_pPhaseToolBar->addAction(m_actionDelPhase);
	connect(m_actionDelPhase, SIGNAL(triggered()), this, SIGNAL(delPhase()));
	m_pPhaseToolBar->addAction(m_actionUpPhase);
	connect(m_actionUpPhase, SIGNAL(triggered()), this, SIGNAL(upPhase()));
	m_pPhaseToolBar->addAction(m_actionDownPhase);
	connect(m_actionDownPhase, SIGNAL(triggered()), this, SIGNAL(downPhase()));

	connect(m_pListView, SIGNAL(clicked(const QModelIndex&)), this, SIGNAL(phaseListViewItemClicked(const QModelIndex&)));
	connect(m_pListView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(phaseClicked()));

	QVBoxLayout *pvbox = new QVBoxLayout;
	pvbox->addWidget(new QLabel("Phases"));
	pvbox->addWidget(m_pListView);
	pvbox->addWidget(m_pPhaseToolBar);
	setLayout(pvbox);
}

void HPhaseListWidget::clearSelection()
{
	m_pListView->selectionModel()->select(m_pListView->selectionModel()->selection(), QItemSelectionModel::Deselect);
	m_actionNewPhase->setEnabled(false);
	m_actionDelPhase->setEnabled(false);
	m_actionUpPhase->setEnabled(false);
	m_actionDownPhase->setEnabled(false);
}

void HPhaseListWidget::connections()
{

}

void HPhaseListWidget::phaseEnabledClicked(bool b)
{
	// set backgrond color for item
	QModelIndexList miList = m_pListView->selectionModel()->selectedIndexes();
	if (miList.size() == 1)
	{
		QVariant v;
		if (b) v = QBrush(Qt::white);
		else   v = QBrush(Qt::gray);
		m_pModel->setData(miList.at(0), v, Qt::BackgroundRole);
	}

}

void HPhaseListWidget::phaseNameChanged(const QString& s)
{
	QModelIndexList miList = m_pListView->selectionModel()->selectedIndexes();
	if (miList.size() == 1)
	{
		m_pModel->setData(miList.at(0), QVariant(s));
	}

}

QString HPhaseListWidget::selectedPhase()
{
	QString s;
	QModelIndexList miList = m_pListView->selectionModel()->selectedIndexes();
	if (miList.size() == 1)
	{
		s = m_pModel->data(miList.at(0), Qt::DisplayRole).toString();
	}
	return s;
}

int HPhaseListWidget::selectedPhaseIndex()
{
	int index = -1;
	QModelIndexList miList = m_pListView->selectionModel()->selectedIndexes();
	if (miList.size() > 0)
	{
		index = miList.at(0).row();
	}
	return index;
}

// Remove phase name from string list model. See  HExperimentMain::delPhase()
void HPhaseListWidget::removePhase(int row)
{
	m_pModel->removeRows(row, 1);
}


int HPhaseListWidget::phaseCount()
{
	return m_pModel->rowCount();
}

void HPhaseListWidget::phaseClicked()
{
	Q_UNUSED(index);
	int i = selectedPhaseIndex();
	int count = phaseCount();

	m_actionNewPhase->setEnabled(true);
	m_actionDelPhase->setEnabled(true);
	if (count > 1 && i > 0) m_actionUpPhase->setEnabled(true);
	else m_actionUpPhase->setEnabled(false);
	if (count > 1 &&  i < count-1) m_actionDownPhase->setEnabled(true);
	else m_actionDownPhase->setEnabled(false);
}

void HPhaseListWidget::movePhaseDown()
{
	QModelIndexList indexList = m_pListView->selectionModel()->selectedIndexes();
	if (indexList.size() == 1)
	{
		if (indexList.at(0).row() < phaseCount()-1)
		{
			// move item at row indexList.at(0).row() down to row indexList.at(0).row()+1
			QModelIndex below = m_pModel->index(indexList.at(0).row()+1);
			QString name = m_pModel->data(below, 0).toString();
			m_pModel->setData(below, m_pModel->data(indexList.at(0), 0));
			m_pModel->setData(indexList.at(0), name);
			m_pListView->setCurrentIndex(below);
		}
	}
}

void HPhaseListWidget::movePhaseUp()
{
	QModelIndexList indexList = m_pListView->selectionModel()->selectedIndexes();
	if (indexList.size() == 1)
	{
		if (indexList.at(0).row() > 0)
		{
			// move item at row indexList.at(0).row() down to row indexList.at(0).row()+1
			QModelIndex above = m_pModel->index(indexList.at(0).row()-1);
			QString name = m_pModel->data(above, 0).toString();
			m_pModel->setData(above, m_pModel->data(indexList.at(0), 0));
			m_pModel->setData(indexList.at(0), name);
			m_pListView->setCurrentIndex(above);
		}
	}
}
