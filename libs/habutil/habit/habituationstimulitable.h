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
	void initializeTable();
	void initializeTable(const Habit::StimuliSettings& stimuliSettings);

	// External interface
public:
	Habit::StimulusSettings* getSelectedStimulus();
	void removeSelectedStimulus();
	bool updateStimulus(const Habit::StimulusSettings&);
	bool addNewStimulus(const Habit::StimulusSettings&);
	QString getSelectedStimulusName();
	Habit::StimulusSettings* findStimulusByName(QString name);
	int findStimulusIndexByName(QString name);
	Habit::StimuliSettings getConfigurationStimuliObject();
	bool isStimulusNameExists(const QString& name);
private:
	Habit::StimulusSettingsList m_listSS;
	const HStimContext* m_pcontext;
	size_t columnCount_;
	QTableWidgetItem*  m_current_item;
};
}
#endif // HABITUATIONSTIMULITABLE_H
