/*
 * HStimulusOrderListWidget.cpp
 *
 *  Created on: Nov 20, 2014
 *      Author: Oakeslab
 */

#include "HStimulusOrderListWidget.h"
#include "HStimulusOrderEditor.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QtDebug>
#include <QStringList>
#include <QMessageBox>
using namespace GUILib;
using namespace Habit;

HStimulusOrderListWidget::HStimulusOrderListWidget(Habit::HStimulusOrderList& list, const Habit::HStimulusSettingsList& ssList, const HStimContext& context, const HStimulusLayoutType& layoutType, QWidget *parent)
: QWidget(parent)
, m_list(list)
, m_ssList(ssList)
, m_pcontext(&context)
, m_playoutType(&layoutType)
{
	create();
	connections();
}

void HStimulusOrderListWidget::setStimulusLayoutType(const HStimulusLayoutType& layoutType)
{
	m_playoutType = &layoutType;
}


void HStimulusOrderListWidget::create()
{
	m_pmodel = new HStimulusOrderListModel(m_list, m_ssList);
	m_pListView = new QListView(this);
	m_pListView->setModel(m_pmodel);

	m_pbNew = new QPushButton("New");
	m_pbEdit = new QPushButton("Edit");
	m_pbRemove = new QPushButton("Remove");
	QVBoxLayout *vPB = new QVBoxLayout;
	vPB->addWidget(m_pbNew);
	vPB->addWidget(m_pbEdit);
	vPB->addWidget(m_pbRemove);
	vPB->addStretch(1);

	// Initial state of edit/remove is disabled
	m_pbEdit->setEnabled(false);
	m_pbRemove->setEnabled(false);

	QHBoxLayout *h = new QHBoxLayout;
	h->addWidget(m_pListView);
	h->addLayout(vPB);
	h->addStretch(1);
	setLayout(h);
}

void HStimulusOrderListWidget::connections()
{
	connect(m_pbEdit, SIGNAL(clicked()), this, SLOT(editClicked()));
	connect(m_pListView, SIGNAL(activated(const QModelIndex&)), this, SLOT(itemActivated(const QModelIndex&)));
	connect(m_pbNew, SIGNAL(clicked()), this, SLOT(newClicked()));
	connect(m_pbRemove, SIGNAL(clicked()), this, SLOT(removeClicked()));
	//connect(m_pListView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(selectionChanged(const QItemSelection&, const QItemSelection&)));
	connect(m_pListView->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), this, SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)));
	//connect(m_pListView->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(currentSelectionChanged(const QModelIndex&, const QModelIndex&)));
	connect(m_pListView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(selectionChanged(const QItemSelection&, const QItemSelection&)));
}

void HStimulusOrderListWidget::removeClicked()
{
	QModelIndexList selected = m_pListView->selectionModel()->selectedIndexes();
	QListIterator<QModelIndex> it(selected);
	while (it.hasNext())
		m_pmodel->remove(it.next());
}

void HStimulusOrderListWidget::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
	//qDebug() << "HStimulusOrderListWidget::selectionChanged - selected: " << selected.count() << " deselected " << deselected.count();
	if (selected.count() > 0)
	{
		// when a stimulus is selected, we should deselect anything currently selected in the stimulus order widget.
		m_pbEdit->setEnabled(true);
		m_pbRemove->setEnabled(true);

		emit orderSelectionChanged();
		emit clearStimulus();
		emit previewOrder(selected.at(0).indexes().at(0).row());
		//emit previewStimulus(selected.at(0).indexes().at(0).row());
	}
	else
	{
		m_pbEdit->setEnabled(false);
		m_pbRemove->setEnabled(false);
	}
}

void HStimulusOrderListWidget::currentSelectionChanged(const QModelIndex& current, const QModelIndex& previous)
{
	// If something is selected, enable the Edit and Remove push buttons
	if (current.row() > -1)
	{
		m_pbEdit->setEnabled(true);
		m_pbRemove->setEnabled(true);
	}
	else
	{
		m_pbEdit->setEnabled(false);
		m_pbRemove->setEnabled(false);
	}
}

void HStimulusOrderListWidget::itemActivated(const QModelIndex& index)
{
	editItem(index);
}


void HStimulusOrderListWidget::editClicked()
{
	// what is current item selected in list view?
	QModelIndexList selectedIndexes = m_pListView->selectionModel()->selectedIndexes();
	if (selectedIndexes.size() == 1)
	{
		editItem(selectedIndexes.at(0));
	}
}




void HStimulusOrderListWidget::editItem(const QModelIndex& index)
{
	HStimulusOrder order(m_list.at(index.row()));
	HStimulusOrderEditor* pEditor = new HStimulusOrderEditor(order, m_ssList, *m_playoutType, this);
	int status = pEditor->exec();
	if (status == QDialog::Accepted)
	{
		m_pmodel->replace(index, pEditor->getOrder());
	}
}

void HStimulusOrderListWidget::newClicked()
{
	HStimulusOrder order(*m_pcontext);
	HStimulusOrderEditor* pEditor = new HStimulusOrderEditor(order, m_ssList, *m_playoutType, this);
	int status = pEditor->exec();
	if (status == QDialog::Accepted)
	{
		m_pmodel->append(pEditor->getOrder());
	}
}


void HStimulusOrderListWidget::append(const Habit::HStimulusOrderList& list)
{
	m_pmodel->append(list);
}

void HStimulusOrderListWidget::append(const Habit::HStimulusOrder& order)
{
	m_pmodel->append(order);
}

void HStimulusOrderListWidget::clobber(const Habit::HStimulusOrder& order)
{
	m_pmodel->clobber(order);
}

void HStimulusOrderListWidget::clearSelection()
{
	m_pListView->clearSelection();
}

