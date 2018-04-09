
#ifndef HABIT_STIMULUS_SETTINGS_H
#define HABIT_STIMULUS_SETTINGS_H

/// \file stimulussettings.h
/// \brief Declaraction of class Habit::StimulusSettings

#include "stimulusinfo.h"
#include "HTypes.h"
#include <QString>
#include <QDataStream>
#include <QTextStream>
#include <QList>

namespace Habit {
	   
	class HStimulusSettingsList;

	/// Stores stimuli settings for a trial.
	class StimulusSettings
	{
	public:
		StimulusSettings();
		StimulusSettings(const StimulusSettings& s);
		StimulusSettings(const QString& name, const QColor& color = Qt::gray);
		~StimulusSettings();
		StimulusSettings clone() const;
		StimulusSettings& operator=(const StimulusSettings& rhs);
		
	public:
		int getId() const;
		void setId(int id);
		const QString& getName() const;
		void setName(const QString& name);
		StimulusInfo& getLeftStimulusInfo();
		const StimulusInfo& getLeftStimulusInfo() const;
		void setLeftStimulusInfo(const StimulusInfo& leftStimulusInfo);
		StimulusInfo& getCenterStimulusInfo();
		const StimulusInfo& getCenterStimulusInfo() const;
		void setCenterStimulusInfo(const StimulusInfo& centerStimulusInfo);
		StimulusInfo& getRightStimulusInfo();
		const StimulusInfo& getRightStimulusInfo() const;
		void setRightStimulusInfo(const StimulusInfo& rightStimulusInfo);
		StimulusInfo& getIndependentSoundInfo();
		const StimulusInfo& getIndependentSoundInfo() const;
		void setIndependentSoundInfo(const StimulusInfo& independentSoundInfo);
		void setStimulusInfo(const StimulusInfo& info, const HPlayerPositionType& position);


		friend bool operator==(const Habit::StimulusSettings&lhs, const Habit::StimulusSettings& rhs);
		friend bool operator!=(const Habit::StimulusSettings&lhs, const Habit::StimulusSettings& rhs);

	private:
		int id_;
		QString name_;
		StimulusInfo leftStimulusInfo_;
		StimulusInfo centerStimulusInfo_;
		StimulusInfo rightStimulusInfo_;
		StimulusInfo independentSoundInfo_;
	};
	QDataStream & operator<< (QDataStream& stream, StimulusSettings settings);
	QDataStream & operator>> (QDataStream& stream, StimulusSettings& settings);
	QTextStream & operator<< (QTextStream& stream, StimulusSettings settings);
	QTextStream & operator>> (QTextStream& stream, StimulusSettings& settings);
	bool operator==(const Habit::StimulusSettings& lhs, const Habit::StimulusSettings& rhs);
	bool operator!=(const Habit::StimulusSettings& lhs, const Habit::StimulusSettings& rhs);


//	typedef QList<StimulusSettings> StimulusSettingsList;
	typedef StimulusSettings HStimulusSettings;	// Sorry, StimulusSettings doesn't have an H- analog.
	typedef HStimulusSettingsList StimulusSettingsList;
	typedef QList<StimulusSettings>::iterator StimulusSettingsListIterator;
	typedef QList<StimulusSettings>::const_iterator StimulusSettingsListConstIterator;
	typedef QList<QPair<int, StimulusSettings> > IdStimulusSettingsPairList;
	typedef QMap<int, StimulusSettings> StimulusSettingsMap;
	typedef QMap<QString, StimulusSettings> StimulusSettingsNameMap;
	typedef QMap<QString, StimulusSettings>::iterator StimulusSettingsNameMapIterator;
	QDebug operator<<(QDebug dbg, const StimulusSettings& ss);
	

} //namespace Habit


#endif // HABIT_STIMULUS_SETTINGS_H
