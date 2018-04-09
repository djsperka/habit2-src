/*
 * HStimulusSettingsListWidget.cpp
 *
 *  Created on: Nov 4, 2014
 *      Author: Oakeslab
 */

#include "HStimulusSettingsListWidget.h"
#include "HStimulusSettingsEditor.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QtDebug>
#include <QStringList>
using namespace GUILib;

HStimulusSettingsListWidget::HStimulusSettingsListWidget(Habit::HStimulusSettingsList& list, const Habit::StimulusDisplayInfo& sdi, QWidget *parent)
: QWidget(parent)
, m_list(list)
, m_sdi(sdi)
{
	create();
	connections();
}


void HStimulusSettingsListWidget::create()
{
	m_pmodel = new HStimulusSettingsListModel(m_list, m_sdi.getStimulusLayoutType());
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

void HStimulusSettingsListWidget::setStimulusLayoutType(const HStimulusLayoutType& layoutType)
{
	m_pmodel->setLayoutType(layoutType);
}


void HStimulusSettingsListWidget::connections()
{
	connect(m_pbEdit, SIGNAL(clicked()), this, SLOT(editClicked()));
	connect(m_pListView, SIGNAL(activated(const QModelIndex&)), this, SLOT(itemActivated(const QModelIndex&)));
	connect(m_pbNew, SIGNAL(clicked()), this, SLOT(newClicked()));
	connect(m_pbRemove, SIGNAL(clicked()), this, SLOT(removeClicked()));
	//connect(m_pListView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(selectionChanged(const QItemSelection&, const QItemSelection&)));
	connect(m_pListView->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), this, SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)));
	//connect(m_pListView->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(currentSelectionChanged(const QModelIndex&, const QModelIndex&)));
	connect(m_pListView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(selectionChanged(const QItemSelection&, const QItemSelection&)));

	connect(m_pmodel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(dataChanged(const QModelIndex &, const QModelIndex &)));
	connect(m_pmodel, SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(rowsInserted(const QModelIndex &, int, int)));
	connect(m_pmodel, SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)), this, SLOT(rowsAboutToBeRemoved(const QModelIndex &, int, int)));
}

void HStimulusSettingsListWidget::removeClicked()
{
	QModelIndexList selected = m_pListView->selectionModel()->selectedIndexes();

	// deselect, otherwise a new selection will be made automatically.
	m_pListView->clearSelection();

	QListIterator<QModelIndex> it(selected);
	while (it.hasNext())
		m_pmodel->remove(it.next());
}

void HStimulusSettingsListWidget::previewClicked()
{
	QModelIndexList selected = m_pListView->selectionModel()->selectedIndexes();
	QListIterator<QModelIndex> it(selected);
	if (selected.size() == 1)
	{
		emit previewStimulus(selected.at(0).row());
	}
}


void HStimulusSettingsListWidget::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
	if (selected.count() > 0)
	{
		// when a stimulus is selected, we should deselect anything currently selected in the stimulus order widget.
		m_pbEdit->setEnabled(true);
		m_pbRemove->setEnabled(true);

		emit stimulusSelectionChanged();	// StimulusOrderListWidget will get this
		//emit clearStimulus();			// unnecesary - REMOVE THIS SIGNAL
		emit previewStimulus(selected.at(0).indexes().at(0).row());
	}
	else
	{
		m_pbEdit->setEnabled(false);
		m_pbRemove->setEnabled(false);
	}
}

void HStimulusSettingsListWidget::currentSelectionChanged(const QModelIndex& current, const QModelIndex& previous)
{
	// If something is selected, enable the Edit and Remove push buttons
	if (current.row() > -1)
	{
		m_pbEdit->setEnabled(true);
		m_pbRemove->setEnabled(true);

		emit clearStimulus();
		emit previewStimulus(current.row());
	}
	else
	{
		m_pbEdit->setEnabled(false);
		m_pbRemove->setEnabled(false);
	}
}

void HStimulusSettingsListWidget::itemActivated(const QModelIndex& index)
{
	editItem(index);
}


void HStimulusSettingsListWidget::editClicked()
{
	// what is current item selected in list view?
	QModelIndexList selectedIndexes = m_pListView->selectionModel()->selectedIndexes();
	if (selectedIndexes.size() == 1)
	{
		editItem(selectedIndexes.at(0));
	}
}

void HStimulusSettingsListWidget::editItem(const QModelIndex& index)
{
	QStringList names;
	QListIterator<Habit::StimulusSettings> it(m_list);
	while (it.hasNext())
		names.append(it.next().getName());

	// Create a new HStimulusSettingsEditor, using the currently selected item
	HStimulusSettingsEditor *pEditor = new HStimulusSettingsEditor(m_list.at(index.row()), m_sdi, names, QString("Modify Stimulus"), this);
	if (pEditor->exec() == QDialog::Accepted)
	{
		Habit::StimulusSettings ss = pEditor->getStimulusSettings();
		if (m_list[index.row()] != ss)
		{
			// update the stimulus settings in the saved list
			m_list[index.row()] = ss;

			// Notify ListView that something has changed
			m_pmodel->changed(index, index);
		}
	}
}

void HStimulusSettingsListWidget::newClicked()
{
	Habit::StimulusSettings newSettings;
	QStringList names;
	QListIterator<Habit::StimulusSettings> it(m_list);
	while (it.hasNext())
		names.append(it.next().getName());

	// Create a new HStimulusSettingsEditor, using the currently selected item
	HStimulusSettingsEditor *pEditor = new HStimulusSettingsEditor(Habit::StimulusSettings(), m_sdi, names, QString("Create New Stimulus"), this);
	if (pEditor->exec() == QDialog::Accepted)
	{
		// append stimulus settings to the StimulusSettingsList
		m_pmodel->append(pEditor->getStimulusSettings());
	}
}


void HStimulusSettingsListWidget::append(const Habit::HStimulusSettingsList& list)
{
	m_pmodel->append(list);
}

void HStimulusSettingsListWidget::append(const Habit::HStimulusSettings& ss)
{
	m_pmodel->append(ss);
}

void HStimulusSettingsListWidget::clobber(const Habit::HStimulusSettings& ss)
{
	m_pmodel->clobber(ss);
}

void HStimulusSettingsListWidget::clearSelection()
{
	m_pListView->clearSelection();
	// get current selection
//	QModelIndexList indexes = m_pListView->selectionModel()->selectedIndexes();
//	QModelIndex index;
//	QItemSelection selection(indexes.at(0), indexes.at(0));
//   m_pListView->selectionModel()->select(selection, QItemSelectionModel::Toggle);
//	foreach(index, indexes)
//	{
//		m_pListView->selectionModel()->select(index, QItemSelectionModel::Toggle);
//		qDebug() << "(de)Selected row " << index.row();
//	}
}


void HStimulusSettingsListWidget::rowsInserted(const QModelIndex &parent, int first, int last)
{
	emit stimulusAdded(first);
}

void HStimulusSettingsListWidget::rowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
	emit stimulusAboutToBeRemoved(first);
}

void HStimulusSettingsListWidget::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
	emit stimulusSettingsChanged(topLeft.row());
}
