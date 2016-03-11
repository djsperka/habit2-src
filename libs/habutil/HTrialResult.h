/*
 * HTrialResult.h
 *
 *  Created on: Mar 10, 2016
 *      Author: Oakeslab
 */

#ifndef HTRIALRESULT_H_
#define HTRIALRESULT_H_

#include "HTrialKey.h"
#include <QList>
#include <QString>
#include "HLook.h"
#include "HEvents.h"


class HTrialResult
{
	HTrialKey m_key;
	QString m_order;
	int m_trialStartTime;
	int m_trialEndTime;
	int m_lookEnabledTime;
	int m_lookDisabledTime;
	bool m_bHabituated;
	QString m_sSubjectID;
	QString m_sOrder;
	QString m_sEndType;
	int m_iStimID;
	QString m_sStimName;
	QString m_sStimLabel;
	QString m_sStimLeft;
	QString m_sStimCenter;
	QString m_sStimRight;
	QString m_sStimISS;
	QList<HLook> m_looks;
	QList<HEvent*> m_events;

public:
	static QStringList headers;
	enum
	{
		indSubjectId = 0,
		indPhase = 1,
		indOrderName = 2,
		indTrial = 3,
		indRepeat = 4,
		indEndType = 5,
		indHabituated = 6,
		indStimId = 7,
		indStimName = 8,
		indStimLabel = 9,
		indStimLeft = 10,
		indStimCenter = 11,
		indStimRight = 12,
		indStimISS = 13,
		indTrialStartTime = 14,
		indTrialEndTime = 15,
		indTotalLook = 16,
		indTotalAway = 17,
		indTotalLookLeft = 18,
		indTotalLookCenter = 19,
		indTotalLookRight = 20,
		indLookEnabledTime = 21,
		indLookDisabledTime = 22,
		indNInit = 23,
	};

	HTrialResult();
	virtual ~HTrialResult();
	HTrialResult(const HTrialKey& key);
	HTrialResult(const HTrialResult&);
	HTrialResult& operator=(const HTrialResult& rhs);

	void setTrialStartTime(int t);
	void setTrialEndTime(int t);
	void setLookEnabledTime(int t);
	void setLookDisabledTime(int t);
	void setLooks(const QList<HLook>& looks);
	void setSubjectID(const QString& s);
	void setOrder(const QString& s);
	void setEndType(const QString& s);
	void setStimId(int i);
	void setStimName(const QString& s);
	void setStimLabel(const QString& s);
	void setStimLeft(const QString& s);
	void setStimCenter(const QString& s);
	void setStimRight(const QString& s);
	void setStimISS(const QString& s);
	void setHabituated(bool b);
	void setOrderName(const QString& s);

	QString getHeader(int header) const;
	QString getString(int header) const;
	const QList<HLook>& looks() const;
	const QList<HEvent*>& events() const;

	void appendLook(const HLook& look);
	void appendEvent(HEvent *e);

#if 0
	void setId(QString id) { (*this)[indSubjectId] = id; };
	void setPhase(QString phase) { (*this)[indPhase] = phase; };
	void setTrial(int itrial) { (*this)[indTrial] = QString("%1").arg(itrial); };
	void setOrderName(QString name) { (*this)[indOrderName] = name; };
	void setRepeat(int irepeat) { (*this)[indRepeat] = QString("%1").arg(irepeat); };
	void setEndType(QString type) { (*this)[indEndType] = type; };
	void setHabituated(QString habituated) { (*this)[indHabituated] = habituated; };
	void setStimId(int stimid) { (*this)[indStimId] = QString("%1").arg(stimid); };
	void setStimName(QString stim) { (*this)[indStimName] = stim; };
	void setStimLabel(QString label) { (*this)[indStimLabel] = label; };
	void setStimLeft(QString stim) { (*this)[indStimLeft] = stim; };
	void setStimCenter(QString stim) { (*this)[indStimCenter] = stim; };
	void setStimRight(QString stim) { (*this)[indStimRight] = stim; };
	void setStimISS(QString stim) { (*this)[indStimISS] = stim; };
	void setTotalLook(QString look) { (*this)[indTotalLook] = look; };
	void setTotalLookAway(QString away) { (*this)[indTotalAway] = away; };
	void setTrialStartTime(int t) { (*this)[indTrialStartTime] = QString("%1").arg(t); m_lastLookEndTime = t; };
	void setTrialEndTime(int t)
	{
		m_trialEndTime = t;
		(*this)[indTrialEndTime] = QString("%1").arg(t);
		(*this)[indTotalLook] = QString("%1").arg(m_totalLookTime);
		(*this)[indTotalAway] = QString("%1").arg(m_totalLookAwayTime);
		(*this)[indTotalLookLeft] = QString("%1").arg(m_totalLookLeftTime);
		(*this)[indTotalLookCenter] = QString("%1").arg(m_totalLookCenterTime);
		(*this)[indTotalLookRight] = QString("%1").arg(m_totalLookRightTime);

	};
	void setLooks(const QList<HLook>& looks_and_incomplete_looks);
	int getTrialEndTime() const { return m_trialEndTime; };
	void setLookEnabledTime(int t) { m_lookEnabledTime = t; };
	int getLookEnabledTime() const { return m_lookEnabledTime; };
	void appendLook(HLook look);
	void updateTotals();
	void appendEvent(HEvent* pevent);
	const QList<HLook>& looks() const { return m_looks; };	// WARNING: includes incomplete looks
	const QList<HEvent*> trans_events() const { return m_listLookTransEvents; }

private:
	int m_lastLookEndTime;
	int m_totalLookTime;
	int m_totalLookAwayTime;
	int m_totalLookLeftTime;
	int m_totalLookCenterTime;
	int m_totalLookRightTime;
	int m_trialEndTime;
	int m_lookEnabledTime;
	QList<HLook> m_looks;
	QList<HEvent*> m_listLookTransEvents;
	HLook m_lookAllLookingPending;	// this is for recording all looking, not necessarily a Look!
	const HLookDirection* m_pdirectionAllLookingPending;
	int m_startMSAllLookingPending;
	void init()
	{
		m_lastLookEndTime = 0;
		m_totalLookTime = 0;
		m_totalLookAwayTime = 0;
		m_totalLookLeftTime = 0;
		m_totalLookCenterTime = 0;
		m_totalLookRightTime = 0;
		m_trialEndTime = 0;
		m_lookEnabledTime = 0;
		clear();
		m_looks.clear();
		m_listLookTransEvents.clear();
		m_lookAllLookingPending.setStartMS(0);
		m_lookAllLookingPending.setEndMS(0);
		m_lookAllLookingPending.setLookMS(0);
		m_lookAllLookingPending.setDirection(HLookDirection::UnknownLookDirection);
		for (int i=0; i<indNInit; i++) append(QString());
	};
#endif

};




#endif /* HTRIALRESULT_H_ */
