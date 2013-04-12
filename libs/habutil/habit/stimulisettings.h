
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
		typedef QVector<Habit::StimulusSettings> stimulus_container;

	public:
		StimuliSettings(const HStimContext& context);
		~StimuliSettings();

	public:
		const HStimContext& getStimContext() const;
		stimulus_container getStimuli() const;
		void setStimuli(const stimulus_container& basis);
		void addStimuli(const StimulusSettings& settings);
		void loadFromDB(int experimentId);
		bool saveToDB(size_t id_);

	private:
		stimulus_container stimuliSettings_;
		const HStimContext* pcontext_;
	};

	QDebug operator<<(QDebug dbg, const StimuliSettings& ss);
	
} //namespace Habit

#endif // HABIT_CRITERION_SETTINGS_H
