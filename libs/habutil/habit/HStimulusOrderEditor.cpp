/*
 * HStimulusOrderEditor.cpp
 *
 *  Created on: Dec 2, 2014
 *      Author: Oakeslab
 */

#include "HStimulusOrderEditor.h"
#include "HStimulusOrderListItemDelegate.h"
#include "ui_HStimulusOrderEditorForm.h"
#include <QModelIndex>
#include <QMessageBox>
#include <QStringList>
using namespace GUILib;
using namespace Habit;

HStimulusOrderEditor::HStimulusOrderEditor(const HStimulusOrder& order, const HStimulusSettingsList& stimlist, const Habit::StimulusDisplayInfo& sdi, QWidget* parent)
: QDialog(parent)
, ui(new Ui::HStimulusOrderEditorForm)
, m_originalOrder(order)
, m_stimlist(stimlist)
, m_sdi(sdi)
{
	components();
	connections();
}

HStimulusOrderEditor::~HStimulusOrderEditor()
{
	delete ui;
	delete m_pStimListModel;
	delete m_pOrderModel;
}

void HStimulusOrderEditor::components()
{
	ui->setupUi(this);
	ui->lineeditName->setText(m_originalOrder.getName());
	m_pStimListModel = new HStimulusSettingsListModel(m_stimlist, m_sdi);
	ui->listviewStimuli->setModel(m_pStimListModel);
	m_pOrderModel = new HStimulusNameListModel(m_originalOrder.getList(), m_stimlist.names());
	ui->listviewOrder->setModel(m_pOrderModel);
	ui->listviewOrder->setItemDelegate(new HStimulusOrderListItemDelegate());

//	m_pOrderModel = new HStimulusNameLabelTableModel(m_originalOrder.getList(), m_stimlist.names());
//	ui->tableviewOrder->setModel(m_pOrderModel);
//	ui->tableviewOrder->horizontalHeader()->setStretchLastSection(true);
//	ui->tableviewOrder->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
//	ui->tableviewOrder->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
//	ui->tableviewOrder->verticalHeader()->setDefaultSectionSize(20);
}

void HStimulusOrderEditor::connections()
{
	connect(ui->pbAdd, SIGNAL(clicked()), this, SLOT(addClicked()));
	connect(ui->pbDone, SIGNAL(clicked()), this, SLOT(doneClicked()));
	connect(ui->pbCancel, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui->pbMoveUp, SIGNAL(clicked()), this, SLOT(moveUpClicked()));
	connect(ui->pbMoveDown, SIGNAL(clicked()), this, SLOT(moveDownClicked()));
	connect(ui->pbRemove, SIGNAL(clicked()), this, SLOT(removeClicked()));
	connect(ui->listviewStimuli, SIGNAL(activated(const QModelIndex&)), this, SLOT(itemActivated(const QModelIndex&)));
}

HStimulusOrder HStimulusOrderEditor::getOrder()
{
	HStimulusOrder order(m_originalOrder);
	order.setName(ui->lineeditName->text());
	order.setList(m_pOrderModel->stringList());
	return order;
}

void HStimulusOrderEditor::itemActivated(const QModelIndex& index)
{
	add(m_stimlist[index.row()].getName());
}

void HStimulusOrderEditor::addClicked()
{
	// Add button was clicked. The currently highlighted item is added to the
	// order after the current item.
	QModelIndexList indexList = ui->listviewStimuli->selectionModel()->selectedIndexes();
	if (!indexList.isEmpty())
	{
		QListIterator<QModelIndex> it(indexList);
		while (it.hasNext())
		{
			QString name(m_stimlist[it.next().row()].getName());
			add(name);
		}
	}
}


void HStimulusOrderEditor::add(const QString& name)
{
	int row;
	if (ui->listviewOrder->currentIndex() == QModelIndex())
	{
		// nothing selected, append
		row = m_pOrderModel->rowCount()-1;
	}
	else
	{
		row = ui->listviewOrder->currentIndex().row();
	}
//	qDebug() << "Add stim " << name << " after row " << row;
	m_pOrderModel->insertRows(row+1, 1);
	QModelIndex index = m_pOrderModel->index(row+1);
	m_pOrderModel->setData(index, name);
	ui->listviewOrder->setCurrentIndex(index);

}
void HStimulusOrderEditor::removeClicked()
{
	// Remove button was clicked.
	// If there is a current index, remove it.
	int row = ui->listviewOrder->currentIndex().row();
	if (row >= 0)
	{
		m_pOrderModel->removeRow(row);
	}
}

void HStimulusOrderEditor::doneClicked()
{
	// must have a name, and it must be unique
	QString proposedName = ui->lineeditName->text();
	if (proposedName.isEmpty())
	{
		QMessageBox::warning(this, "Bad Order Name", "Order name is empty. Please enter a valid name.");
	}
	else
	{
		// look through the list of names. At most one occurrence is allowed, if two are found
		// there is a duplicate.
		bool bNoGood = false;
		QStringList allStimNames(m_stimlist.names());
		for (int i=0; i<allStimNames.size(); i++)
			if (allStimNames[i]==proposedName && allStimNames[i]!=m_originalOrder.getName()) bNoGood = true;
		if (bNoGood)
		{
			QMessageBox::warning(this, "Bad Order Name", "Duplicate order name. Please enter a valid name.");
		}
		else
		{
			accept();
		}
	}
}

void HStimulusOrderEditor::moveUpClicked()
{
	QModelIndexList indexList = ui->listviewOrder->selectionModel()->selectedIndexes();
	if (indexList.size() == 1)
	{
		if (indexList.at(0).row() > 0)
		{
			// move item at row indexList.at(0).row() up to row indexList.at(0).row()-1
			QModelIndex above = m_pOrderModel->index(indexList.at(0).row()-1);
			QString name = m_pOrderModel->data(above, 0).toString();
			m_pOrderModel->setData(above, m_pOrderModel->data(indexList.at(0), 0));
			m_pOrderModel->setData(indexList.at(0), name);
			ui->listviewOrder->setCurrentIndex(above);
		}
	}
}

void HStimulusOrderEditor::moveDownClicked()
{
	QModelIndexList indexList = ui->listviewOrder->selectionModel()->selectedIndexes();
	if (indexList.size() == 1)
	{
		if (indexList.at(0).row() < m_pOrderModel->rowCount()-1)
		{
			// move item at row indexList.at(0).row() down to row indexList.at(0).row()+1
			QModelIndex below = m_pOrderModel->index(indexList.at(0).row()+1);
			QString name = m_pOrderModel->data(below, 0).toString();
			m_pOrderModel->setData(below, m_pOrderModel->data(indexList.at(0), 0));
			m_pOrderModel->setData(indexList.at(0), name);
			ui->listviewOrder->setCurrentIndex(below);
		}
	}
}
