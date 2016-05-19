
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

namespace Habit
{
	class StimuliSettings
	{
	public:
		StimuliSettings();
		StimuliSettings(const StimuliSettings& s);
		~StimuliSettings();
		StimuliSettings& operator=(const StimuliSettings& rhs);
		StimuliSettings clone();

	public:
		HStimulusSettingsList& stimuli();
		const HStimulusSettingsList& stimuli() const;
		void setStimuli(const HStimulusSettingsList& basis);
		void addStimulus(const HStimulusSettings& settings);
		void setOrderList(const HStimulusOrderList& list);
		void addOrder(const HStimulusOrder& order);
		HStimulusOrderList& orders();
		const HStimulusOrderList& orders() const;
		void loadFromDB(int phaseId);
		void saveToDB(int phaseId);

		QStringList getStimulusNames() const;
		QStringList getOrderNames() const;
		bool getIndexedOrderList(const QString& orderName, QList< QPair<int, QString> >& list) const;

	private:
		HStimulusSettingsList ssList_;
		HStimulusOrderList soList_;
	};

	QDebug operator<<(QDebug dbg, const StimuliSettings& ss);
	QDataStream & operator<< (QDataStream& stream, StimuliSettings settings);
	QDataStream & operator>> (QDataStream& stream, StimuliSettings& settings);
	bool operator==(const Habit::StimuliSettings&lhs, const Habit::StimuliSettings& rhs);
	
} //namespace Habit

#endif // HABIT_CRITERION_SETTINGS_H
