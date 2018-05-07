/*
 * HExperimentListWidget.cpp
 *
 *  Created on: Sep 5, 2014
 *      Author: Oakeslab
 */


#include "HExperimentListWidget.h"
#include "HExperimentListModel.h"
#include "HExperimentListDelegate.h"
#include <QtDebug>
#include "maindao.h"


GUILib::HExperimentListWidget::HExperimentListWidget(QWidget *parent, bool bExp, bool bTemp)
: QListView(parent)
{
	setModel(m_pModel = new GUILib::HExperimentListModel(bExp, bTemp));
	setEditTriggers(QAbstractItemView::SelectedClicked);
	this->setItemDelegate(new HExperimentListDelegate());
	connect(this, SIGNAL(activated(const QModelIndex&)), this, SLOT(itemActivated(const QModelIndex&)));
	//connect(m_pModel, SIGNAL(dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)), this, SLOT(dataChanged(const QModelIndex& topLeft, const QModelIndex & bottomRight)));
}

QStringList GUILib::HExperimentListWidget::experimentList()
{
	return m_pModel->stringList();
}

void GUILib::HExperimentListWidget::reload()
{
	m_pModel->reload();
}

GUILib::HExperimentListWidget::~HExperimentListWidget()
{
	delete m_pModel;
}

QString GUILib::HExperimentListWidget::selectedExperiment()
{
	QString expt;
	QModelIndexList selectedIndexes = this->selectionModel()->selectedIndexes();
	if (selectedIndexes.size() == 1)
	{
		expt = m_pModel->stringList().at(selectedIndexes.at(0).row());
	}
	return expt;

}

void GUILib::HExperimentListWidget::itemActivated(const QModelIndex&)
{
	emit experimentActivated(selectedExperiment());
}

