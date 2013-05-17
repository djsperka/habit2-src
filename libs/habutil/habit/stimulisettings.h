
#ifndef HABIT_STIMULI_SETTINGS_H
#define HABIT_STIMULI_SETTINGS_H

/// \file stimulisettings.h
/// \brief Declaraction of class Habit::StimuliSettings

#include "stimulussettings.h"
#include "HTypes.h"
#include <QtCore/QVector>
#include <QDebug>

/// Common namespace for all entities of the Habit
namespace Habit
{
   
	/// Stores the list of the stimulus settings.
	class StimuliSettings
	{
	public:
		// djs The preferred way of storing a list of StimulusSettings is StimulusSettingsList (see stimulussettings.h)
		//typedef QVector<Habit::StimulusSettings> stimulus_container;

	public:
		StimuliSettings(const HStimContext& context);
		~StimuliSettings();

	public:
		const HStimContext& getStimContext() const;
		void setStimContext(const HStimContext& context);
		StimulusSettingsList getStimuli() const;
		void setStimuli(const StimulusSettingsList& basis);
		void addStimuli(const StimulusSettings& settings);
		void loadFromDB(int experimentId);
		bool saveToDB(size_t id_);

	private:
		StimulusSettingsList ssList_;
		const HStimContext* pcontext_;
	};

	QDebug operator<<(QDebug dbg, const StimuliSettings& ss);
	QDataStream & operator<< (QDataStream& stream, StimuliSettings settings);
	QDataStream & operator>> (QDataStream& stream, StimuliSettings& settings);
	bool operator==(const Habit::StimuliSettings&lhs, const Habit::StimuliSettings& rhs);
	
} //namespace Habit

#endif // HABIT_CRITERION_SETTINGS_H
