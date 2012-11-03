/*
 *  HEvents.h
 *  hstate
 *
 *  Created by Oakes Lab on 6/7/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _HEVENTS_H_
#define _HEVENTS_H_

#include <QString>
#include <QTextStream>
#include "stimulussettings.h"
#include "stimulusinfo.h"
#include "HLook.h"


namespace habit2
{
	enum HEventType
	{
		kPhaseStart = 0,
		kPhaseEnd,
		kTrialStart,
		kTrialEnd,
		kStimRequest,
		kStimStart,
		kStimulusSettings,
		kStimulusInfo,
		kAttention,
		kLook,
		kUndefined
	};
	
	enum HTrialEndType
	{
		kTrialEndSuccess,
		kTrialEndAbort,
		kTrialEndFail
	};
		
	
	QTextStream& operator<<(QTextStream& out, const HTrialEndType& etype);
	QTextStream& operator<<(QTextStream& out, const HEventType& type);
		
	
	class HEvent
	{
	public:
		HEvent(HEventType type, int timestamp=0)
		: m_type(type)
		, m_timestamp(timestamp)
		{};		
		virtual ~HEvent() {};

		// default constructor and copy constructor
		HEvent()
		: m_type(kUndefined)
		, m_timestamp(0)
		{};
		
		HEvent(const HEvent& e)
		: m_type(e.type())
		, m_timestamp(e.timestamp())
		{};
				 
		
		int timestamp() const { return m_timestamp; };
		HEventType type() const { return m_type; };
		
		// Each event should give pretty output of its content. NOT the type or time!
		virtual QString eventInfo() const = 0;
		
		friend QTextStream& operator<<(QTextStream& out, const HEvent& e);
		
	private:
		HEventType m_type;
		int m_timestamp;
	};

	QTextStream& operator<<(QTextStream& out, const HEvent& e);

	class HPhaseStartEvent: public HEvent
	{
	public:
		HPhaseStartEvent(QString& phase, int timestamp = 0) 
		: HEvent(kPhaseStart, timestamp)
		, m_phase(phase)
		{};

		HPhaseStartEvent(char* phase, int timestamp = 0) 
		: HEvent(kPhaseStart, timestamp)
		, m_phase(phase)
		{};
		
		HPhaseStartEvent()
		: HEvent(kPhaseStart)
		, m_phase("Unknown")
		{};
		
		HPhaseStartEvent(const HPhaseStartEvent& e)
		: HEvent(e.type(), e.timestamp())
		, m_phase("Unknown")
		{};
		
		virtual ~HPhaseStartEvent() {};
		const QString& phase() const { return m_phase; };
		QString eventInfo() const;
	private:
		QString m_phase;
	};

	class HPhaseEndEvent: public HEvent
	{
	public:
		HPhaseEndEvent(int timestamp = 0) 
		: HEvent(kPhaseEnd, timestamp)
		{};
		
		HPhaseEndEvent(const HPhaseEndEvent& e)
		: HEvent(kPhaseEnd, e.timestamp())
		{};
		
		~HPhaseEndEvent() {};
		
		QString eventInfo() const;
	};
	
	class HTrialStartEvent: public HEvent
	{
	public:
		HTrialStartEvent(int trialnumber, int repeatnumber=0, int timestamp=0)
		: HEvent(kTrialStart, timestamp)
		, m_trialnumber(trialnumber)
		, m_repeatnumber(repeatnumber)
		{};
		
		HTrialStartEvent()
		: HEvent(kTrialStart)
		, m_trialnumber(-1)
		, m_repeatnumber(-1)
		{};
		
		HTrialStartEvent(const HTrialStartEvent& e)
		: HEvent(kTrialStart, e.timestamp())
		, m_trialnumber(e.trialnumber())
		, m_repeatnumber(e.repeatnumber())
		{};
		
		virtual ~HTrialStartEvent() {};
		QString eventInfo() const;
		int trialnumber() const { return m_trialnumber; };
		int repeatnumber() const { return m_trialnumber; };
	private:
		int m_trialnumber;
		int m_repeatnumber;
	};

	class HTrialEndEvent: public HEvent
	{
	public:
		HTrialEndEvent(HTrialEndType endtype = kTrialEndSuccess, int timestamp = 0) 
		: HEvent(kTrialEnd, timestamp)
		, m_endtype(endtype)
		{};
		
		HTrialEndEvent(const HTrialEndEvent& e)
		: HEvent(kPhaseEnd, e.timestamp())
		, m_endtype(e.endtype())
		{};
		
		virtual ~HTrialEndEvent() {};
		HTrialEndType endtype() const { return m_endtype; };
		QString eventInfo() const;
	private:
		HTrialEndType m_endtype;
	};	
	
	class HStimRequestEvent: public HEvent
	{
	public:
		HStimRequestEvent(int stimindex=-2, int timestamp=0)
		: HEvent(kStimRequest, timestamp)
		, m_stimindex(stimindex)
		{};
		
		HStimRequestEvent(const HStimRequestEvent& e)
		: HEvent(kStimRequest, e.timestamp())
		, m_stimindex(e.stimindex())
		{};
		
		virtual ~HStimRequestEvent() {};
		
		QString eventInfo() const;
		int stimindex() const { return m_stimindex; };
	private:
		int m_stimindex;
	};
	
	class HStimStartEvent: public HEvent
	{
	public:
		HStimStartEvent(int stimindex=-2, int playerid=-1, int timestamp=0)
		: HEvent(kStimStart, timestamp)
		, m_stimindex(stimindex)
		, m_playerid(playerid)
		{};
		
		HStimStartEvent(const HStimStartEvent& e)
		: HEvent(kStimStart, e.timestamp())
		, m_stimindex(e.stimindex())
		, m_playerid(e.playerid())
		{};
		
		virtual ~HStimStartEvent() {};
		
		QString eventInfo() const;
		int stimindex() const { return m_stimindex; };
		int playerid() const { return m_playerid; };
	private:
		int m_stimindex;
		int m_playerid;
	};
	
	// TODO: how do you pass the settings arg as a ref? 
	class HStimulusSettingsEvent: public HEvent
	{
	public:
		HStimulusSettingsEvent(Habit::StimulusSettings settings = Habit::StimulusSettings(), int stimindex=-2, int timestamp=0)
		: HEvent(kStimulusSettings, timestamp)
		, m_settings(settings)
		, m_stimindex(stimindex)
		{};
		
		HStimulusSettingsEvent(const HStimulusSettingsEvent& e)
		: HEvent(kStimulusSettings, e.timestamp())
		, m_settings(e.settings())
		, m_stimindex(e.stimindex())
		{};
		
		virtual ~HStimulusSettingsEvent() {};
		
		const Habit::StimulusSettings& settings() const { return m_settings; };
		int stimindex() const { return m_stimindex; };
		QString eventInfo() const;
	private:
		Habit::StimulusSettings m_settings;
		int m_stimindex;
	};
	
	class HStimulusInfoEvent: public HEvent
	{
	public:
		HStimulusInfoEvent(QString whichstim = QString("Unknown"), Habit::StimulusInfo info = Habit::StimulusInfo(), int stimindex=-2, int timestamp=0)
		: HEvent(kStimulusInfo, timestamp)
		, m_whichstim(whichstim)
		, m_info(info)
		, m_stimindex(stimindex)
		{};
		
		HStimulusInfoEvent(const HStimulusInfoEvent& e)
		: HEvent(kStimulusInfo, e.timestamp())
		, m_whichstim(e.whichstim())
		, m_info(e.info())
		, m_stimindex(e.stimindex())
		{};
		
		virtual ~HStimulusInfoEvent() {};
		
		const QString& whichstim() const { return m_whichstim; };
		const Habit::StimulusInfo& info() const { return m_info; };
		int stimindex() const { return m_stimindex; };
		QString eventInfo() const;
	private:
		QString m_whichstim;
		Habit::StimulusInfo m_info;
		int m_stimindex;
	};
	
	class HAttentionEvent: public HEvent
	{
	public:
		HAttentionEvent(int timestamp=0)
		: HEvent(kAttention, timestamp)
		{};
		
		HAttentionEvent(const HAttentionEvent& e)
		: HEvent(kAttention, e.timestamp())
		{};
		
		virtual ~HAttentionEvent() {};
		
		QString eventInfo() const;
	};
	
	class HLookEvent: public HEvent
	{
	public:
		HLookEvent(HLook look = HLook(), int timestamp=0)
		: HEvent(kLook, timestamp)
		, m_look(look)
		{};
		
		HLookEvent(const HLookEvent& e)
		: HEvent(kLook, e.timestamp())
		, m_look(e.look())
		{};
		
		virtual ~HLookEvent() {};
		
		const HLook& look() const { return m_look; };
		QString eventInfo() const;
		
	private:
		HLook m_look;
	};
		
};



#endif