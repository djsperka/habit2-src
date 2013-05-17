
#ifndef HABIT_STIMULUS_SETTINGS_H
#define HABIT_STIMULUS_SETTINGS_H

/// \file stimulussettings.h
/// \brief Declaraction of class Habit::StimulusSettings

#include "stimulusinfo.h"
#include "HTypes.h"
#include <QString>
#include <QDataStream>
#include <QList>

namespace Habit {
	   
	/// Stores stimuli settings for a trial.
	class StimulusSettings
	{
	public:
		StimulusSettings(const QString& name = "", const HStimContext& context = HStimContext::UnknownContext);
		StimulusSettings(const StimulusSettings& s);
		~StimulusSettings();
		
	public:
		int getId() const;
		void setId(int id);
		const QString& getName() const;
		void setName(const QString& name);
		bool isLeftEnabled() const;
		void setLeftEnabled(bool isLeftEnabled);
		bool isCenterEnabled() const;
		void setCenterEnabled(bool isCenterEnabled);
		bool isRightEnabled() const;
		void setRightEnabled(bool isRightEnabled);
		bool isIndependentSoundEnabled() const;
		void setIndependentSoundEnabled(bool isRightEnabled);
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
		const HStimContext* getContext() const;
		void setContext(const HStimContext& context);

		friend bool operator==(const Habit::StimulusSettings&lhs, const Habit::StimulusSettings& rhs);

	private:
		int id_;
		QString name_;
		bool isLeftEnabled_;
		bool isCenterEnabled_;
		bool isRightEnabled_;
		bool isIndependentSoundEnabled_;
		StimulusInfo leftStimulusInfo_;
		StimulusInfo centerStimulusInfo_;
		StimulusInfo rightStimulusInfo_;
		StimulusInfo independentSoundInfo_;
		const HStimContext* pcontext_;
	};
	QDataStream & operator<< (QDataStream& stream, StimulusSettings settings);
	QDataStream & operator>> (QDataStream& stream, StimulusSettings& settings);
	bool operator==(const Habit::StimulusSettings&lhs, const Habit::StimulusSettings& rhs);

	typedef QList<StimulusSettings> StimulusSettingsList;
	typedef QList<StimulusSettings>::iterator StimulusSettingsListIterator;
	typedef QList<StimulusSettings>::const_iterator StimulusSettingsListConstIterator;
	typedef QList<QPair<int, StimulusSettings> > IdStimulusSettingsPairList;
	typedef QMap<int, StimulusSettings> StimulusSettingsMap;
	typedef QMap<QString, StimulusSettings> StimulusSettingsNameMap;
	typedef QMap<QString, StimulusSettings>::iterator StimulusSettingsNameMapIterator;
	QDebug operator<<(QDebug dbg, const StimulusSettings& ss);
	

} //namespace Habit


#endif // HABIT_STIMULUS_SETTINGS_H
