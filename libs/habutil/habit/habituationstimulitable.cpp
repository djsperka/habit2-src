#include "habituationstimulitable.h"
#include "stimulussettings.h"

#include <QtGui/QWidget>
#include <QtGui/QHeaderView>
#include <QtGui/QTableWidget>
#include <QtGui/QTableWidgetItem>
#include <QtGui/QMessageBox>

namespace GUILib {
HabituationStimuliTable::HabituationStimuliTable(const Habit::StimuliSettings& settings, QWidget* w)
	: QTableWidget(w)
	, settings_(settings)
{
	//setFixedSize(538, 382);
	setColumnCount(1);
	initializeTable(settings);
	setSelectionMode(QAbstractItemView::SingleSelection);
}

void HabituationStimuliTable::initializeTable(const Habit::StimuliSettings& ss){
	clear();
	setRowCount(0);
	m_container.clear();
	settings_ = ss;
	QStringList header;
	header << "Stimulus Name";
	setHorizontalHeaderLabels(header);
	horizontalHeader()->setStretchLastSection(true);
	qDebug() << "HabituationStimuliTable::initializeTable context " << ss.getStimContext().name();
	typedef Habit::StimuliSettings::stimulus_container M;
	M stimulusSettings =  settings_.getStimuli();
	M::iterator i;
	int row = 0;
	for (i = stimulusSettings.begin(); i != stimulusSettings.end(); ++i) {
		Habit::StimulusSettings ss = *i;
		if(!m_container.contains(ss.getName())) {
			m_container.insert(ss.getName(), ss);
			QTableWidgetItem* item = new QTableWidgetItem(ss.getName());
			qDebug() << "HabituationStimuliTable::initializeTable " << ss.getName();
			item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
			if(row >= rowCount()) {
				setRowCount(row + 1);
			}
			setItem(row, 0, item);
			row += 1;
		} else {
			// TODO: Handle the case when there are two stimulus with the same name
		}
	} 
}

HabituationStimuliTable::~HabituationStimuliTable()	{
}

QString HabituationStimuliTable::getSelectedStimulusName() {
	QString name;
	typedef QList<QTableWidgetItem*> M;
	M selected_items = selectedItems();
	if(selectedItems().size() == 1) {
		QTableWidgetItem* item = selected_items[0];
		name = item->text();
		m_current_item = item;
	}
	return name;
}

Habit::StimulusSettings* HabituationStimuliTable::getSelectedStimulus(){
	QString stimulus_name = getSelectedStimulusName();
	if(!(stimulus_name.isNull() && stimulus_name.isEmpty())) {
		stimulus_container::iterator it = m_container.find(stimulus_name);
		if(it != m_container.end()) {
			return &(*it);
		}
	}
	return 0;
}

void HabituationStimuliTable::removeSelectedStimulus(){
	QString stimulus_name = getSelectedStimulusName();
	if(!(stimulus_name.isNull() && stimulus_name.isEmpty())) {
		Q_ASSERT(m_container.end() != m_container.find(stimulus_name));
		m_container.remove(stimulus_name);
		for(unsigned i = rowCount(); i > 0; --i) {
			QTableWidgetItem* it = item(i - 1, 0);
			if(0 != it && it->text() == stimulus_name) {
				removeRow(i - 1);
				return;
			}
		}
	}
}

bool HabituationStimuliTable::updateStimulus(const Habit::StimulusSettings& ss) {
	bool result = false;
	if(!ss.getName().trimmed().isEmpty()) {
		if(0 != m_current_item) {
			stimulus_container::iterator it = m_container.find(ss.getName());
			if((m_container.end() != it && (*it).getName() == m_current_item->text()) || m_container.end() == it) {
				m_container.remove(m_current_item->text());
				m_container.insert(ss.getName(), ss);
				m_current_item->setText(ss.getName());
				result = true;
			}
		}
	}
	return result;
}

bool HabituationStimuliTable::addNewStimulus(const Habit::StimulusSettings& settings) {
	bool result = false;
	if(!settings.getName().isEmpty() && m_container.end() == m_container.find(settings.getName())) {
		m_container.insert(settings.getName(), settings);
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
	return !(!name.isEmpty() && m_container.end() == m_container.find(name));
}

Habit::StimuliSettings HabituationStimuliTable::getConfigurationStimuliObject() 
{
	Habit::StimuliSettings settings(settings_.getStimContext());
	stimulus_container::const_iterator i = m_container.begin();
	for ( ; i != m_container.end(); ++i) 
	{
		settings.addStimuli(i.value());
	}
	return settings;
}
	
}
