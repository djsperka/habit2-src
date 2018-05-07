/*
 * HStimulusSettingsEditor.cpp
 *
 *  Created on: Nov 5, 2014
 *      Author: Oakeslab
 */

#include "HStimulusSettingsEditor.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QtDebug>

using namespace GUILib;

HStimulusSettingsEditor::HStimulusSettingsEditor(const Habit::StimulusSettings& settings, const Habit::StimulusDisplayInfo& sdi, const QStringList& names, const QString& title, QWidget *parent)
: QDialog(parent)
, m_originalSettings(settings)
, m_names(names)
{
	m_pStimulusSettingsWidget = new HStimulusSettingsWidget(settings, sdi, parent);
	setWindowTitle(title);

	m_pbDone = new QPushButton("Done");
	m_pbCancel = new QPushButton("Cancel");
	QHBoxLayout *hButtons = new QHBoxLayout;
	hButtons->addStretch(1);
	hButtons->addWidget(m_pbCancel);
	hButtons->addWidget(m_pbDone);

	QVBoxLayout* v = new QVBoxLayout();
	v->addWidget(m_pStimulusSettingsWidget);
	v->addLayout(hButtons);
	setLayout(v);

	connect(m_pbCancel, SIGNAL(clicked()), this, SLOT(reject()));
	connect(m_pbDone, SIGNAL(clicked()), this, SLOT(doneClicked()));
}


HStimulusSettingsEditor::HStimulusSettingsEditor(const Habit::StimulusSettings& settings, const HPlayerPositionType& ppt, const QStringList& names, const QString& title, QWidget *parent)
: QDialog(parent)
, m_originalSettings(settings)
, m_names(names)
{
	m_pStimulusSettingsWidget = new HStimulusSettingsWidget(settings, ppt, parent);
	setWindowTitle(title);

	m_pbDone = new QPushButton("Done");
	m_pbCancel = new QPushButton("Cancel");
	QHBoxLayout *hButtons = new QHBoxLayout;
	hButtons->addStretch(1);
	hButtons->addWidget(m_pbCancel);
	hButtons->addWidget(m_pbDone);

	QVBoxLayout* v = new QVBoxLayout();
	v->addWidget(m_pStimulusSettingsWidget);
	v->addLayout(hButtons);
	setLayout(v);

	connect(m_pbCancel, SIGNAL(clicked()), this, SLOT(reject()));
	connect(m_pbDone, SIGNAL(clicked()), this, SLOT(doneClicked()));
}

Habit::StimulusSettings HStimulusSettingsEditor::getStimulusSettings()
{
	return m_pStimulusSettingsWidget->getStimulusSettings();
}

void HStimulusSettingsEditor::doneClicked()
{
	// verify that the settings are kosher. If so, call accept();
	QString proposedName = m_pStimulusSettingsWidget->getName();
	if (proposedName.isEmpty())
	{
		QMessageBox::warning(this, "Bad Stimulus Name", "Stimulus name is empty. Please enter a valid name.");
	}
	else
	{
		// look through the list of names. At most one occurrence is allowed, if two are found
		// there is a duplicate.
		bool bNoGood = false;
		for (int i=0; i<m_names.size(); i++)
			if (m_names[i]==proposedName && m_names[i]!=m_originalSettings.getName()) bNoGood = true;
		if (bNoGood)
		{
			QMessageBox::warning(this, "Bad Stimulus Name", "Duplicate stimulus name. Please enter a valid name.");
		}
		else
		{
			accept();
		}
	}
}
