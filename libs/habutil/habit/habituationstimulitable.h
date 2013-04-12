#ifndef HABITUATIONSTIMULITABLE_H
#define HABITUATIONSTIMULITABLE_H

#include "stimulisettings.h"

#include <QtGui/QTableWidget>

class QTableWidget;

namespace GUILib {

class HabituationStimuliTable : public QTableWidget
{
	Q_OBJECT

public:
	typedef QMap<QString, Habit::StimulusSettings> stimulus_container;

public:
	explicit HabituationStimuliTable(const Habit::StimuliSettings& settings, QWidget* w = 0);
	~HabituationStimuliTable();

public:
	void initializeTable(const Habit::StimuliSettings& ss);

	// External interface
public:
	Habit::StimulusSettings* getSelectedStimulus();
	void removeSelectedStimulus();
	bool updateStimulus(const Habit::StimulusSettings&);
	bool addNewStimulus(const Habit::StimulusSettings&);
	QString getSelectedStimulusName();
	Habit::StimuliSettings getConfigurationStimuliObject();
	bool isStimulusNameExists(const QString& name);
private:
	stimulus_container m_container;
	size_t columnCount_;
	Habit::StimuliSettings settings_;
	QTableWidgetItem*  m_current_item;
};
}
#endif // HABITUATIONSTIMULITABLE_H
