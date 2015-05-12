
#ifndef HABIT_STIMULI_SETTINGS_H
#define HABIT_STIMULI_SETTINGS_H

/// \file stimulisettings.h
/// \brief Declaraction of class Habit::StimuliSettings

#include "stimulussettings.h"
#include "HStimulusSettingsList.h"
#include "HStimulusOrder.h"
#include "HStimulusOrderList.h"
#include "HTypes.h"
#include <QtCore/QVector>
#include <QDebug>
#include <QStringList>

/// Common namespace for all entities of the Habit
namespace Habit
{
   
	/// Stores the list of the stimulus settings.
	class StimuliSettings
	{
	public:
		StimuliSettings(const HStimContext& context);
		StimuliSettings(const StimuliSettings& s);
		~StimuliSettings();
		StimuliSettings& operator=(const StimuliSettings& rhs);
		StimuliSettings clone();

	public:
		const HStimContext& getStimContext() const;
		void setStimContext(const HStimContext& context);
		HStimulusSettingsList& stimuli();
		const HStimulusSettingsList& stimuli() const;
		void setStimuli(const HStimulusSettingsList& basis);
		void addStimulus(const HStimulusSettings& settings);
		void setOrderList(const HStimulusOrderList& list);
		void addOrder(const HStimulusOrder& order);
		HStimulusOrderList& orders();
		const HStimulusOrderList& orders() const;
		void loadFromDB(int experimentId);
		bool saveToDB(size_t id_);

		QStringList getStimulusNames() const;
		QStringList getOrderNames() const;
		bool getIndexedOrderList(const QString& orderName, QList< QPair<int, QString> >& list) const;

	private:
		HStimulusSettingsList ssList_;
		HStimulusOrderList soList_;
		const HStimContext* pcontext_;
	};

	QDebug operator<<(QDebug dbg, const StimuliSettings& ss);
	QDataStream & operator<< (QDataStream& stream, StimuliSettings settings);
	QDataStream & operator>> (QDataStream& stream, StimuliSettings& settings);
	bool operator==(const Habit::StimuliSettings&lhs, const Habit::StimuliSettings& rhs);
	
} //namespace Habit

#endif // HABIT_CRITERION_SETTINGS_H
