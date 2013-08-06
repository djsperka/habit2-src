#include "habituationstimulitable.h"
#include "stimulussettings.h"

#ifndef USING_QT5
#include <QtGui>
#else
#include <QtWidgets>
#endif

namespace GUILib {
HabituationStimuliTable::HabituationStimuliTable(const Habit::StimuliSettings& settings, QWidget* w)
	: QTableWidget(w)
	, m_listSS(settings.getStimuli())
	, m_pcontext(&settings.getStimContext())
{
	setColumnCount(1);
	initializeTable();
	setSelectionMode(QAbstractItemView::SingleSelection);
}

void HabituationStimuliTable::initializeTable()
{
	QStringList header;
	int row = 0;
	clear();
	setRowCount(0);
	header << "Stimulus Name";
	setHorizontalHeaderLabels(header);
	horizontalHeader()->setStretchLastSection(true);
	qDebug() << "HabituationStimuliTable::initializeTable context " << m_pcontext->name();
	for (Habit::StimulusSettingsListConstIterator it = m_listSS.begin(); it!=m_listSS.end(); it++)
	{
		QTableWidgetItem* item = new QTableWidgetItem(it->getName());
		qDebug() << "HabituationStimuliTable::initializeTable stim=" << it->getName();
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
		if(row >= rowCount())
		{
			setRowCount(row + 1);
		}
		setItem(row, 0, item);
		row += 1;
	}
}

void HabituationStimuliTable::initializeTable(const Habit::StimuliSettings& stimuliSettings)
{
	m_pcontext = &stimuliSettings.getStimContext();
	m_listSS.clear();
	m_listSS.append(stimuliSettings.getStimuli());
	initializeTable();
}



HabituationStimuliTable::~HabituationStimuliTable()
{
}

QString HabituationStimuliTable::getSelectedStimulusName()
{
	QString name;
	typedef QList<QTableWidgetItem*> M;
	M selected_items = selectedItems();
	if(selectedItems().size() == 1)
	{
		QTableWidgetItem* item = selected_items[0];
		name = item->text();
		m_current_item = item;
	}
	return name;
}


int HabituationStimuliTable::findStimulusIndexByName(QString name)
{
	int i;
	int index = -1;
	for (i=0; i<m_listSS.size(); i++)
	{
		if (m_listSS[i].getName() == name)
		{
			index = i;
			break;
		}
	}
	return index;
}


Habit::StimulusSettings* HabituationStimuliTable::findStimulusByName(QString name)
{
	bool bFound = false;
	Habit::StimulusSettings* pstim = NULL;
	Habit::StimulusSettingsListIterator it;
	for (it = m_listSS.begin(); it != m_listSS.end() && !bFound; it++)
	{
		if (it->getName() == name)
		{
			bFound = true;
			pstim = &(*it);
		}
	}
	return pstim;
}

Habit::StimulusSettings* HabituationStimuliTable::getSelectedStimulus()
{
	return findStimulusByName(getSelectedStimulusName());
}

void HabituationStimuliTable::removeSelectedStimulus()
{
	QString stimulus_name = getSelectedStimulusName();
	Habit::StimulusSettings* pstim = NULL;
	if (pstim)
	{
		m_listSS.removeOne(*pstim);
	}
	for(unsigned i = rowCount(); i > 0; --i)
	{
		QTableWidgetItem* it = item(i - 1, 0);
		if(0 != it && it->text() == stimulus_name)
		{
			removeRow(i - 1);
			return;
		}
	}
}

bool HabituationStimuliTable::updateStimulus(const Habit::StimulusSettings& ss) {
	bool result = false;
	int index;
	if(!ss.getName().trimmed().isEmpty())
	{
		index = findStimulusIndexByName(ss.getName());
		if (0 != m_current_item && index >= 0)
		{
			m_listSS.replace(index, ss);
			m_current_item->setText(ss.getName());
			result = true;
		}
	}
	return result;
}

bool HabituationStimuliTable::addNewStimulus(const Habit::StimulusSettings& settings)
{
	bool result = false;
	if(!settings.getName().isEmpty() && findStimulusIndexByName(settings.getName()) < 0)
	{
		m_listSS.append(settings);
		int row = rowCount();
		setRowCount(row + 1);
		QTableWidgetItem* item = new QTableWidgetItem(settings.getName());
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
		setItem(row, 0, item);
		result = true;
	}
	return result;
}

bool HabituationStimuliTable::isStimulusNameExists(const QString& name)
{
	return !(!name.isEmpty() && findStimulusIndexByName(name) > -1);
}

Habit::StimuliSettings HabituationStimuliTable::getConfigurationStimuliObject() 
{
	Habit::StimuliSettings settings(*m_pcontext);
	settings.setStimuli(m_listSS);
	return settings;
}
	
}
