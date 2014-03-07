/*
 * HTrialSettings.h
 *
 *  Created on: Feb 26, 2014
 *      Author: Oakeslab
 */

#ifndef HTRIALSETTINGS_H_
#define HTRIALSETTINGS_H_

#include <QDataStream>
#include "HTypes.h"

namespace Habit
{
	class HPhaseSettings
	{
	public:
		HPhaseSettings(const HPhaseType& type = HPhaseType::UnknownPhase);
		HPhaseSettings(const HPhaseSettings& ts);
		HPhaseSettings& operator=(const HPhaseSettings& rhs);

		int getId() const { return m_id; };
		void setId(int id) { m_id = id; };
		bool getIsEnabled() const { return m_bEnabled; };
		void setIsEnabled(bool b) { m_bEnabled = b; };
		const HPhaseType& getPhaseType() const { return *m_pPhaseType; };
		void setPhaseType(const HPhaseType& ptype) { m_pPhaseType = &ptype; };
		unsigned int getNTrials() const { return m_ntrials; };
		void setNTrials(unsigned int n) { m_ntrials = n; };
		bool getUseLookingCriteria() const { return m_bUseLookingCriteria; };
		void setUseLookingCriteria(bool b) { m_bUseLookingCriteria = b; };
		bool getIsSingleLook() const { return m_bIsSingleLook; };
		void setIsSingleLook(bool b) { m_bIsSingleLook = b; };
		bool getIsMaxAccumulatedLookTime() const { return m_bIsMaxAccumulatedLookTime; };
		void setIsMaxAccumulatedLookTime(bool b) { m_bIsMaxAccumulatedLookTime = b; };
		unsigned int getMaxAccumulatedLookTime() const { return m_uiMaxAccumulatedLookTime; };
		void setMaxAccumulatedLookTime(unsigned int ui) { m_uiMaxAccumulatedLookTime = ui; };
		bool getIsMaxLookAwayTime() const { return m_bIsMaxLookAwayTime; };
		void setIsMaxLookAwayTime(bool b) { m_bIsMaxLookAwayTime = b; };
		unsigned int getMaxLookAwayTime() const { return m_uiMaxLookAwayTime; };
		void setMaxLookAwayTime(unsigned int ui) { m_uiMaxLookAwayTime = ui; };
		bool getIsMaxStimulusTime() const { return m_bIsMaxStimulusTime; };
		void setIsMaxStimulusTime(bool b) { m_bIsMaxStimulusTime = b; };
		unsigned int getMaxStimulusTime() const { return m_uiMaxStimulusTime; };
		void setMaxStimulusTime(unsigned int ui) { m_uiMaxStimulusTime = ui; };
		bool getMeasureStimulusTimeFromOnset() const { return m_bMeasureStimulusTimeFromOnset; };
		void setMeasureStimulusTimeFromOnset(bool b) { m_bMeasureStimulusTimeFromOnset = b; };
		bool getMeasureStimulusTimeFromLooking() const { return m_bMeasureStimulusTimeFromLooking; };
		void setMeasureStimulusTimeFromLooking(bool b) { m_bMeasureStimulusTimeFromLooking = b; };
		bool getIsMaxNoLookTime() const { return m_bIsMaxNoLookTime; };
		void setIsMaxNoLookTime(bool b) { m_bIsMaxNoLookTime = b; };
		unsigned int getMaxNoLookTime() const { return m_uiMaxNoLookTime; };
		void setMaxNoLookTime(unsigned int ui) { m_uiMaxNoLookTime = ui; };

		void loadFromDB(int id, int type);
		bool saveToDB(int id) const;

	private:
		int m_id;
		bool m_bEnabled;
		const HPhaseType* m_pPhaseType;
		unsigned int m_ntrials;
		bool m_bUseLookingCriteria;	// if true, then must select either single look or accum look time
		bool m_bIsSingleLook;	// trials end on completion of a single look
		bool m_bIsMaxAccumulatedLookTime; 	// trials end when max accumulated look time reached
		unsigned int m_uiMaxAccumulatedLookTime;
		bool m_bIsMaxLookAwayTime;		// trials end if a look away lasts for max look away time
		unsigned int m_uiMaxLookAwayTime;
		bool m_bIsMaxStimulusTime;		// trials can end if stimulus is on for max stim time
		unsigned int m_uiMaxStimulusTime;
		bool m_bMeasureStimulusTimeFromOnset;	// max stim time measured from initial onset
		bool m_bMeasureStimulusTimeFromLooking;	// max stim time measured from initial looking
		bool m_bIsMaxNoLookTime;		// trials can end if initial period of no looking
		unsigned int m_uiMaxNoLookTime;
	};


	QDataStream & operator<< (QDataStream& stream, HPhaseSettings settings);
	QDataStream & operator>> (QDataStream& stream, HPhaseSettings& settings);
	bool operator==(const HPhaseSettings& lhs, const HPhaseSettings& rhs);
}; // namespace

#endif /* HPHASESETTINGS_H_ */
