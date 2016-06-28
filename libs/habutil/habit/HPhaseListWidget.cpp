/*
 * HPhaseListWidget.cpp
 *
 *  Created on: Jun 24, 2016
 *      Author: dan
 */

#include "HPhaseListWidget.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QStringListModel>
#include <QLabel>

using namespace GUILib;

GUILib::HPhaseListWidget::HPhaseListWidget(const QStringList& phaseNames, QWidget* parent)
: QWidget(parent)
{
	components();
	m_pListView->setModel(new QStringListModel(phaseNames));
	connections();
}

GUILib::HPhaseListWidget::~HPhaseListWidget()
{
}

void  GUILib::HPhaseListWidget::components()
{
	m_pListView = new QListView(this);

	m_pPhaseToolBar = new QToolBar();
	m_actionNewPhase = new QAction(QIcon(":/resources/expt-main/p-add.png"), "Add new phase", this);
	m_actionDelPhase = new QAction(QIcon(":/resources/expt-main/p-delete.png"), "Delete selected phase", this);
	m_actionUpPhase = new QAction(QIcon(":/resources/expt-main/p-up.png"), "Move selected phase up", this);
	m_actionDownPhase = new QAction(QIcon(":/resources/expt-main/p-down.png"), "Move selected phase down", this);
	m_actionEditPhase = new QAction(QIcon(":/resources/expt-main/p-edit.png"), "Edit selected phase", this);
	m_pPhaseToolBar->addAction(m_actionNewPhase);
	connect(m_actionNewPhase, SIGNAL(triggered()), this, SIGNAL(addPhase()));
	m_pPhaseToolBar->addAction(m_actionDelPhase);
	connect(m_actionDelPhase, SIGNAL(triggered()), this, SIGNAL(delPhase()));
	m_pPhaseToolBar->addAction(m_actionEditPhase);
	connect(m_actionEditPhase, SIGNAL(triggered()), this, SIGNAL(editPhase()));
	m_pPhaseToolBar->addAction(m_actionUpPhase);
	connect(m_actionUpPhase, SIGNAL(triggered()), this, SIGNAL(upPhase()));
	m_pPhaseToolBar->addAction(m_actionDownPhase);
	connect(m_actionDownPhase, SIGNAL(triggered()), this, SIGNAL(downPhase()));

	connect(m_pListView, SIGNAL(clicked(const QModelIndex&)), this, SIGNAL(phaseListViewItemClicked(const QModelIndex&)));

	QVBoxLayout *pvbox = new QVBoxLayout;
	pvbox->addWidget(new QLabel("Phases"));
	pvbox->addWidget(m_pListView);
	pvbox->addWidget(m_pPhaseToolBar);
	setLayout(pvbox);
}

void HPhaseListWidget::clearSelection()
{
	m_pListView->selectionModel()->select(m_pListView->selectionModel()->selection(), QItemSelectionModel::Deselect);
}

void HPhaseListWidget::connections()
{

}

QString HPhaseListWidget::selectedPhase()
{
	QString s;
	QModelIndexList miList = m_pListView->selectionModel()->selectedIndexes();
	if (miList.size() == 1)
	{
		QStringListModel *model = dynamic_cast<QStringListModel*>(m_pListView->model());
		s = model->data(miList.at(0), Qt::DisplayRole).toString();
	}
	return s;
}
