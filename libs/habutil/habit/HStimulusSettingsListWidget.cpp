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

HStimulusSettingsListWidget::HStimulusSettingsListWidget(Habit::HStimulusSettingsList& list, const HStimulusLayoutType& layoutType, QWidget *parent)
: QWidget(parent)
, m_list(list)
, m_pLayoutType(&layoutType)
{
	create();
	connections();
}


void HStimulusSettingsListWidget::create()
{
	m_pmodel = new HStimulusSettingsListModel(m_list, *m_pLayoutType);
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
	m_pLayoutType = &layoutType;
}


void HStimulusSettingsListWidget::connections()
{
	connect(m_pbEdit, SIGNAL(clicked()), this, SLOT(editClicked()));
	connect(m_pListView, SIGNAL(activated(const QModelIndex&)), this, SLOT(itemActivated(const QModelIndex&)));
	connect(m_pbNew, SIGNAL(clicked()), this, SLOT(newClicked()));
	connect(m_pbRemove, SIGNAL(clicked()), this, SLOT(removeClicked()));
	connect(m_pListView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(selectionChanged(const QItemSelection&, const QItemSelection&)));
}

void HStimulusSettingsListWidget::removeClicked()
{
	QModelIndexList selected = m_pListView->selectionModel()->selectedIndexes();
	QListIterator<QModelIndex> it(selected);
	while (it.hasNext())
		m_pmodel->remove(it.next());
}

void HStimulusSettingsListWidget::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	Q_UNUSED(deselected);
	// If something is selected, enable the Edit and Remove push buttons
	m_pbEdit->setDisabled(selected.isEmpty());
	m_pbRemove->setDisabled(selected.isEmpty());
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
	HStimulusSettingsEditor *pEditor = new HStimulusSettingsEditor(m_list.at(index.row()), *m_pLayoutType, names, QString("Modify Stimulus"), this);
	if (pEditor->exec() == QDialog::Accepted)
	{
		// update the stimulus settings in the saved list
		m_list[index.row()] = pEditor->getStimulusSettings();

		// Notify ListView that something has changed
		m_pmodel->changed(index, index);
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
	HStimulusSettingsEditor *pEditor = new HStimulusSettingsEditor(Habit::StimulusSettings(), *m_pLayoutType, names, QString("Create New Stimulus"), this);
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
