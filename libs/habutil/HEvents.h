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
#include <QDataStream>
#include "stimulussettings.h"
#include "stimulusinfo.h"
#include "HLook.h"
#include "HTypes.h"


/*
 * These are event types for the HEventLog. They are NOT events in the Qt sense - 
 * they don't derive from QEvent!!! 
 * There is a set of QEvent derived events in HQEvents.h
 */


class HEventType
{
public:
	static const HEventType HEventPhaseStart;
	static const HEventType HEventPhaseEnd;
	static const HEventType HEventTrialStart;
	static const HEventType HEventTrialEnd;
	static const HEventType HEventAGRequest;
	static const HEventType HEventAGStart;
	static const HEventType HEventAGEnd;
	static const HEventType HEventStimRequest;
	static const HEventType HEventStimStart;
	static const HEventType HEventStimEnd;
	static const HEventType HEventStimulusSettings;
	static const HEventType HEventStimulusOrder;
	static const HEventType HEventStimulusInfo;
	static const HEventType HEventAttention;
	static const HEventType HEventLook;
	static const HEventType HEventLookTrans;
	static const HEventType HEventUndefined;
	static const HEventType HHabituationSuccess;
	static const HEventType HHabituationFailure;
	static const HEventType HEventReliabilityStart;
	static const HEventType HEventReliabilityEnd;
	static const HEventType HEventExperimentStart;
	static const HEventType HEventExperimentEnd;
	static const HEventType HEventLookEnabled;
	static const HEventType HEventAGLookEnabled;
	static const HEventType HEventLookDisabled;
	static const HEventType HEventScreenStart;

	// undefined event not in A[]
	static const HEventType* A[25];
	
	int number() const { return m_t; }
	const QString& name() const { return m_s; }
	friend bool operator==(const HEventType& lhs, const HEventType& rhs);
	friend bool operator!=(const HEventType& lhs, const HEventType& rhs);
	
private:
	explicit HEventType(int t, const char* s): m_t(t), m_s(s) {};
	int m_t;
	QString m_s;
};	

const HEventType& getHEventType(int number_value);


// These are supplied when creating HTrialEndEvent

class HTrialEndType
{
public:
	static const HTrialEndType HTrialEndGotLook;
	static const HTrialEndType HTrialEndMaxStimulusTime;
	static const HTrialEndType HTrialEndNoLookTimeout;
	static const HTrialEndType HTrialEndAbort;
	static const HTrialEndType HTrialEndMaxAccumulatedLookTime;
	static const HTrialEndType HTrialEndMaxLookAwayTime;
	static const HTrialEndType HTrialEndUndefined;
	
	// Undefined type not in A
	static const HTrialEndType* A[6];
	
	int number() const { return m_t; }
	const QString& name() const { return m_s; }
	friend bool operator==(const HTrialEndType& lhs, const HTrialEndType& rhs);
	
private:
	explicit HTrialEndType(int t, const char* s): m_t(t), m_s(s) {};
	int m_t;
	QString m_s;
};
	
const HTrialEndType& getHTrialEndType(int number_value);

QTextStream& operator<<(QTextStream& out, const HTrialEndType& etype);
QTextStream& operator<<(QTextStream& out, const HEventType& type);
	

class HEvent
{
public:
	HEvent(const HEventType& type, int timestamp=0)
	: m_ptype(&type)
	, m_timestamp(timestamp)
	{};		
	virtual ~HEvent() {};

	// default constructor and copy constructor
	HEvent()
	: m_ptype(&HEventType::HEventUndefined)
	, m_timestamp(0)
	{};
	
	HEvent(const HEvent& e)
	: m_ptype(&e.type())
	, m_timestamp(e.timestamp())
	{};
			 
	
	int timestamp() const { return m_timestamp; };
	const HEventType& type() const { return *m_ptype; };
	void setEventType(const HEventType& type) { m_ptype = &type; };
	
	// Each event should give pretty output of its content. NOT the type or time!
	virtual QString eventInfo() const = 0;
	
	// Each event should also give pretty CSV output of its content. Default behavior
	// is to add nothing. When overriding, omit the leading comma, and double-quote
	// strings.
	QString eventCSV() const;
	virtual QString eventCSVAdditional() const;		// override this to add more fields
	
	friend QTextStream& operator<<(QTextStream& out, const HEvent& e);

	// base class insertion operator will put out the event type number() and timestamp. Then the method putAdditional()
	// is called. Derived classes should override the default putAdditional() (which does nothing).
	// When reading events, the type and timestamp are first read.
	virtual QDataStream& putAdditional(QDataStream& stream) const;

	// clone the event. All derived classes must do this!
	// Cloning can also be done with a new timestamp - as it is done in reliability.
	// If timestamp is negative, use the timestamp in this event.
	virtual HEvent* clone(int timestamp = 0) const = 0;

	// We will have to read without an overloaded >>. I think this isn't possible with a pure virtual base class.
	// Should return NULL on failure. May throw exception....TODO handle this
	static HEvent* getEvent(QDataStream& stream);

private:
	const HEventType* m_ptype;
	int m_timestamp;
};

bool operator==(const HEvent& lhs, const HEvent& rhs);
QTextStream& operator<<(QTextStream& out, const HEvent& e);
QDataStream& operator<<(QDataStream& out, const HEvent& e);

class HPhaseStartEvent: public HEvent
{
public:
	HPhaseStartEvent(const HPhaseType& type, int timestamp = 0) 
	: HEvent(HEventType::HEventPhaseStart, timestamp)
	, m_pphasetype(&type)
	{};

	HPhaseStartEvent()
	: HEvent(HEventType::HEventPhaseStart)
	, m_pphasetype(&HPhaseType::UnknownPhase)
	{};
	
	HPhaseStartEvent(const HPhaseStartEvent& e)
	: HEvent(e.type(), e.timestamp())
	, m_pphasetype(&e.phasetype())
	{};
	
	virtual ~HPhaseStartEvent() {};
	QString phase() const { return m_pphasetype->name(); };
	const HPhaseType& phasetype() const { return *m_pphasetype; };
	QString eventInfo() const;
	
	virtual QString eventCSVAdditional() const;		// override this to add more fields

	virtual QDataStream& putAdditional(QDataStream& stream) const;

	static HPhaseStartEvent* getEvent(QDataStream& stream, int timestamp);

	HEvent* clone(int ts = 0) const
	{
		int t = (ts < 0 ? timestamp() : ts);
		return new HPhaseStartEvent(phasetype(), t);
	};

private:
	const HPhaseType* m_pphasetype;
};

class HPhaseEndEvent: public HEvent
{
public:
	HPhaseEndEvent(int timestamp = 0) 
	: HEvent(HEventType::HEventPhaseEnd, timestamp)
	{};
	
	HPhaseEndEvent(const HPhaseEndEvent& e)
	: HEvent(HEventType::HEventPhaseEnd, e.timestamp())
	{};
	
	~HPhaseEndEvent() {};
	
	QString eventInfo() const;
	static HPhaseEndEvent* getEvent(QDataStream& stream, int timestamp);

	HEvent* clone(int ts = 0) const
	{
		int t = (ts < 0 ? timestamp() : ts);
		return new HPhaseEndEvent(t);
	};
};

class HTrialStartEvent: public HEvent
{
public:
	HTrialStartEvent(int trialnumber, int repeatnumber=0, int timestamp=0)
	: HEvent(HEventType::HEventTrialStart, timestamp)
	, m_trialnumber(trialnumber)
	, m_repeatnumber(repeatnumber)
	{};
	
	HTrialStartEvent()
	: HEvent(HEventType::HEventTrialStart)
	, m_trialnumber(-1)
	, m_repeatnumber(-1)
	{};
	
	HTrialStartEvent(const HTrialStartEvent& e)
	: HEvent(HEventType::HEventTrialStart, e.timestamp())
	, m_trialnumber(e.trialnumber())
	, m_repeatnumber(e.repeatnumber())
	{};
	
	virtual ~HTrialStartEvent() {};
	QString eventInfo() const;
	virtual QString eventCSVAdditional() const;		// override this to add more fields
	virtual QDataStream& putAdditional(QDataStream& stream) const;
	static HTrialStartEvent* getEvent(QDataStream& stream, int timestamp);
	int trialnumber() const { return m_trialnumber; };
	int repeatnumber() const { return m_repeatnumber; };

	HEvent* clone(int ts = 0) const
	{
		int t = (ts < 0 ? timestamp() : ts);
		return new HTrialStartEvent(trialnumber(), repeatnumber(), t);
	};
private:
	int m_trialnumber;
	int m_repeatnumber;
};

class HTrialEndEvent: public HEvent
{
public:
	HTrialEndEvent(const HTrialEndType& endtype = HTrialEndType::HTrialEndUndefined, int timestamp = 0) 
	: HEvent(HEventType::HEventTrialEnd, timestamp)
	, m_pendtype(&endtype)
	{};
	
	HTrialEndEvent(const HTrialEndEvent& e)
	: HEvent(HEventType::HEventTrialEnd, e.timestamp())
	, m_pendtype(&e.endtype())
	{};
	
	virtual ~HTrialEndEvent() {};
	const HTrialEndType& endtype() const { return *m_pendtype; };
	void setEndType(const HTrialEndType& endtype) { m_pendtype = &endtype; };
	QString eventInfo() const;
	virtual QString eventCSVAdditional() const;
	virtual QDataStream& putAdditional(QDataStream& stream) const;
	static HTrialEndEvent* getEvent(QDataStream& stream, int timestamp);

	HEvent* clone(int ts = 0) const
	{
		int t = (ts < 0 ? timestamp() : ts);
		return new HTrialEndEvent(endtype(), t);
	};

private:
	const HTrialEndType* m_pendtype;
};	

class HStimRequestEvent: public HEvent
{
public:
	HStimRequestEvent(int stimindex=-2, int timestamp=0)
	: HEvent(HEventType::HEventStimRequest, timestamp)
	, m_stimindex(stimindex)
	{};
	
	HStimRequestEvent(const HStimRequestEvent& e)
	: HEvent(HEventType::HEventStimRequest, e.timestamp())
	, m_stimindex(e.stimindex())
	{};
	
	virtual ~HStimRequestEvent() {};
	
	QString eventInfo() const;
	virtual QString eventCSVAdditional() const;
	int stimindex() const { return m_stimindex; };

	virtual QDataStream& putAdditional(QDataStream& stream) const;
	static HStimRequestEvent* getEvent(QDataStream& stream, int timestamp);

	HEvent* clone(int ts = 0) const
	{
		int t = (ts < 0 ? timestamp() : ts);
		return new HStimRequestEvent(stimindex(), t);
	};

private:
	int m_stimindex;
};


class HAGRequestEvent: public HEvent
{
public:
	HAGRequestEvent(int timestamp=0)
	: HEvent(HEventType::HEventAGRequest, timestamp)
	{};

	HAGRequestEvent(const HAGRequestEvent& e)
	: HEvent(HEventType::HEventAGRequest, e.timestamp())
	{};

	virtual ~HAGRequestEvent() {};

	QString eventInfo() const;
	//virtual QString eventCSVAdditional() const;
	static HAGRequestEvent* getEvent(QDataStream& stream, int timestamp);

	HEvent* clone(int ts = 0) const
	{
		int t = (ts < 0 ? timestamp() : ts);
		return new HAGRequestEvent(t);
	};

private:
};

class HAGStartEvent: public HEvent
{
public:
	HAGStartEvent(int playerid = -1, int timestamp = 0)
	: HEvent(HEventType::HEventAGStart, timestamp)
	, m_playerid(playerid)
	{};
	
	HAGStartEvent(const HAGStartEvent& e)
	: HEvent(HEventType::HEventAGStart, e.timestamp())
	, m_playerid(e.playerid())
	{};
	
	QString eventInfo() const;
	virtual QString eventCSVAdditional() const;
	int playerid() const { return m_playerid; };

	virtual QDataStream& putAdditional(QDataStream& stream) const;
	static HAGStartEvent* getEvent(QDataStream& stream, int timestamp);

	HEvent* clone(int ts = 0) const
	{
		int t = (ts < 0 ? timestamp() : ts);
		return new HAGStartEvent(playerid(), t);
	};

private:
	int m_playerid;
};

class HAGEndEvent: public HEvent
{
public:
	HAGEndEvent(int playerid = -1, int timestamp = 0)
	: HEvent(HEventType::HEventAGStart, timestamp)
	, m_playerid(playerid)
	{};

	HAGEndEvent(const HAGStartEvent& e)
	: HEvent(HEventType::HEventAGStart, e.timestamp())
	, m_playerid(e.playerid())
	{};

	QString eventInfo() const;
	virtual QString eventCSVAdditional() const;
	int playerid() const { return m_playerid; };

	virtual QDataStream& putAdditional(QDataStream& stream) const;
	static HAGEndEvent* getEvent(QDataStream& stream, int timestamp);

	HEvent* clone(int ts = 0) const
	{
		int t = (ts < 0 ? timestamp() : ts);
		return new HAGEndEvent(playerid(), t);
	};

private:
	int m_playerid;
};



class HStimStartEvent: public HEvent
{
public:
	HStimStartEvent(int stimid=-1, int timestamp=0)
	: HEvent(HEventType::HEventStimStart, timestamp)
	, m_stimid(stimid)
	{};
	
	HStimStartEvent(const HStimStartEvent& e)
	: HEvent(HEventType::HEventStimStart, e.timestamp())
	, m_stimid(e.stimid())
	{};
	
	virtual ~HStimStartEvent() {};
	
	QString eventInfo() const;
	virtual QString eventCSVAdditional() const;
	int stimid() const { return m_stimid; };

	virtual QDataStream& putAdditional(QDataStream& stream) const;
	static HStimStartEvent* getEvent(QDataStream& stream, int timestamp);

	HEvent* clone(int ts = 0) const
	{
		int t = (ts < 0 ? timestamp() : ts);
		return new HStimStartEvent(stimid(), t);
	};

private:
	int m_stimid;
};


class HStimEndEvent: public HEvent
{
public:
	HStimEndEvent(int playerid=-1, int timestamp=0)
	: HEvent(HEventType::HEventStimEnd, timestamp)
	, m_playerid(playerid)
	{};

	HStimEndEvent(const HStimEndEvent& e)
	: HEvent(HEventType::HEventStimEnd, e.timestamp())
	, m_playerid(e.playerid())
	{};

	virtual ~HStimEndEvent() {};

	QString eventInfo() const;
	virtual QString eventCSVAdditional() const;
	int playerid() const { return m_playerid; };

	virtual QDataStream& putAdditional(QDataStream& stream) const;
	static HStimEndEvent* getEvent(QDataStream& stream, int timestamp);

	HEvent* clone(int ts = 0) const
	{
		int t = (ts < 0 ? timestamp() : ts);
		return new HStimEndEvent(playerid(), t);
	};

private:
	int m_playerid;
};

// TODO: how do you pass the settings arg as a ref? 
class HStimulusSettingsEvent: public HEvent
{
public:
	HStimulusSettingsEvent(Habit::StimulusSettings settings = Habit::StimulusSettings(), int stimindex=-2, int timestamp=0)
	: HEvent(HEventType::HEventStimulusSettings, timestamp)
	, m_settings(settings)
	, m_stimindex(stimindex)
	{};
	
	HStimulusSettingsEvent(const HStimulusSettingsEvent& e)
	: HEvent(HEventType::HEventStimulusSettings, e.timestamp())
	, m_settings(e.settings())
	, m_stimindex(e.stimindex())
	{};
	
	virtual ~HStimulusSettingsEvent() {};
	
	const Habit::StimulusSettings& settings() const { return m_settings; };
	int stimindex() const { return m_stimindex; };
	QString eventInfo() const;
	QString eventCSVAdditional() const;
	virtual QDataStream& putAdditional(QDataStream& stream) const;
	static HStimulusSettingsEvent* getEvent(QDataStream& stream, int timestamp);

	HEvent* clone(int ts = 0) const
	{
		int t = (ts < 0 ? timestamp() : ts);
		return new HStimulusSettingsEvent(settings(), stimindex(), t);
	};

private:
	Habit::StimulusSettings m_settings;
	int m_stimindex;
};

class HStimulusOrderEvent: public HEvent
{
public:
	HStimulusOrderEvent(const HPhaseType& type, const QList<unsigned int>& list, int timestamp = 0)
	: HEvent(HEventType::HEventStimulusOrder, timestamp)
	, m_pphasetype(&type)
	, m_list(list)
	{};

	HStimulusOrderEvent()
	: HEvent(HEventType::HEventStimulusOrder)
	, m_pphasetype(&HPhaseType::UnknownPhase)
	, m_list()
	{};

	HStimulusOrderEvent(const HStimulusOrderEvent& e)
	: HEvent(e.type(), e.timestamp())
	, m_pphasetype(&e.phasetype())
	, m_list(e.list())
	{};

	virtual ~HStimulusOrderEvent() {};
	QString phase() const { return m_pphasetype->name(); };
	const HPhaseType& phasetype() const { return *m_pphasetype; };
	const QList<unsigned int>& list() const { return m_list; };
	QString listToString() const;
	QString eventInfo() const;

	virtual QString eventCSVAdditional() const;		// override this to add more fields

	virtual QDataStream& putAdditional(QDataStream& stream) const;

	static HStimulusOrderEvent* getEvent(QDataStream& stream, int timestamp);

	HEvent* clone(int ts = 0) const
	{
		int t = (ts < 0 ? timestamp() : ts);
		return new HStimulusOrderEvent(phasetype(), list(), t);
	};

private:
	const HPhaseType* m_pphasetype;
	QList<unsigned int> m_list;
};

class HStimulusInfoEvent: public HEvent
{
public:
	HStimulusInfoEvent(QString whichstim = QString("Unknown"), Habit::StimulusInfo info = Habit::StimulusInfo(), int stimindex=-2, int timestamp=0)
	: HEvent(HEventType::HEventStimulusInfo, timestamp)
	, m_whichstim(whichstim)
	, m_info(info)
	, m_stimindex(stimindex)
	{};
	
	HStimulusInfoEvent(const HStimulusInfoEvent& e)
	: HEvent(HEventType::HEventStimulusInfo, e.timestamp())
	, m_whichstim(e.whichstim())
	, m_info(e.info())
	, m_stimindex(e.stimindex())
	{};
	
	virtual ~HStimulusInfoEvent() {};
	
	const QString& whichstim() const { return m_whichstim; };
	const Habit::StimulusInfo& info() const { return m_info; };
	int stimindex() const { return m_stimindex; };
	QString eventInfo() const;

	HEvent* clone(int ts = 0) const
	{
		int t = (ts < 0 ? timestamp() : ts);
		return new HStimulusInfoEvent(whichstim(), info(), stimindex(), t);
	};

private:
	QString m_whichstim;
	Habit::StimulusInfo m_info;
	int m_stimindex;
};

class HAttentionEvent: public HEvent
{
public:
	HAttentionEvent(int timestamp=0)
	: HEvent(HEventType::HEventAttention, timestamp)
	{};
	
	HAttentionEvent(const HAttentionEvent& e)
	: HEvent(HEventType::HEventAttention, e.timestamp())
	{};
	
	virtual ~HAttentionEvent() {};
	
	QString eventInfo() const;

	// not needed for this class virtual QDataStream& putAdditional(QDataStream& stream) const;
	static HAttentionEvent* getEvent(QDataStream& stream, int timestamp);

	HEvent* clone(int ts = 0) const
	{
		int t = (ts < 0 ? timestamp() : ts);
		return new HAttentionEvent(t);
	};

};

class HLookEvent: public HEvent
{
public:
	HLookEvent(HLook look = HLook(), int timestamp=0)
	: HEvent(HEventType::HEventLook, timestamp)
	, m_look(look)
	{};
	
	HLookEvent(const HLookEvent& e)
	: HEvent(HEventType::HEventLook, e.timestamp())
	, m_look(e.look())
	{};
	
	virtual ~HLookEvent() {};
	
	const HLook& look() const { return m_look; };
	QString eventInfo() const;
	virtual QString eventCSVAdditional() const;

	virtual QDataStream& putAdditional(QDataStream& stream) const;
	static HLookEvent* getEvent(QDataStream& stream, int timestamp);

	HEvent* clone(int ts = 0) const
	{
		int t = (ts < 0 ? timestamp() : ts);
		return new HLookEvent(look(), t);
	};

private:
	HLook m_look;
};
	

class HLookTransEvent: public HEvent
{
public:
	HLookTransEvent(const HLookTrans& lt = HLookTrans::UnknownLookTrans, int timestamp=0)
	: HEvent(HEventType::HEventLookTrans, timestamp)
	, m_type(lt)
	{};
	
	HLookTransEvent(const HLookTransEvent& e)
	: HEvent(HEventType::HEventLookTrans, e.timestamp())
	, m_type(e.transtype())
	{};
	
	virtual ~HLookTransEvent() {};
	
	const HLookTrans& transtype() const { return m_type; };
	QString eventInfo() const;
	virtual QString eventCSVAdditional() const;

	virtual QDataStream& putAdditional(QDataStream& stream) const;
	static HLookTransEvent* getEvent(QDataStream& stream, int timestamp);

	HEvent* clone(int ts = 0) const
	{
		int t = (ts < 0 ? timestamp() : ts);
		return new HLookTransEvent(transtype(), t);
	};

private:
	const HLookTrans& m_type;
};

class HHabituationSuccessEvent: public HEvent
{
public:
	HHabituationSuccessEvent(int timestamp = 0)
	: HEvent(HEventType::HHabituationSuccess, timestamp)
	{};
	
	HHabituationSuccessEvent(const HHabituationSuccessEvent& e)
	: HEvent(HEventType::HHabituationSuccess, e.timestamp())
	{};
	
	virtual ~HHabituationSuccessEvent() {};
	
	QString eventInfo() const;

	// not needed for this event type: virtual QDataStream& putAdditional(QDataStream& stream) const;
	static HHabituationSuccessEvent* getEvent(QDataStream& stream, int timestamp);

	HEvent* clone(int ts = 0) const
	{
		int t = (ts < 0 ? timestamp() : ts);
		return new HHabituationSuccessEvent(t);
	};

private:
};

class HHabituationFailureEvent: public HEvent
{
public:
	HHabituationFailureEvent(int timestamp = 0)
	: HEvent(HEventType::HHabituationFailure, timestamp)
	{};
	
	HHabituationFailureEvent(const HHabituationFailureEvent& e)
	: HEvent(HEventType::HHabituationFailure, e.timestamp())
	{};
	
	virtual ~HHabituationFailureEvent() {};
	
	QString eventInfo() const;

	// not needed for this event type: virtual QDataStream& putAdditional(QDataStream& stream) const;
	static HHabituationFailureEvent* getEvent(QDataStream& stream, int timestamp);

	HEvent* clone(int ts = 0) const
	{
		int t = (ts < 0 ? timestamp() : ts);
		return new HHabituationFailureEvent(t);
	};

private:
};


class HExperimentStartEvent: public HEvent
{
public:
	HExperimentStartEvent(int timestamp)
	: HEvent(HEventType::HEventExperimentStart, timestamp)
	{};

	HExperimentStartEvent(const HExperimentStartEvent& e)
	: HEvent(HEventType::HEventExperimentStart, e.timestamp())
	{};

	virtual ~HExperimentStartEvent() {};

	QString eventInfo() const;
	//virtual QDataStream& putAdditional(QDataStream& stream) const;
	static HExperimentStartEvent* getEvent(QDataStream& stream, int timestamp);
	//virtual QString eventCSVAdditional() const;


	HEvent* clone(int ts = 0) const
	{
		int t = (ts < 0 ? timestamp() : ts);
		return new HExperimentStartEvent(t);
	};

private:
};

class HExperimentEndEvent: public HEvent
{
public:
	HExperimentEndEvent(int timestamp)
	: HEvent(HEventType::HEventExperimentEnd, timestamp)
	{};

	HExperimentEndEvent(const HExperimentEndEvent& e)
	: HEvent(HEventType::HEventExperimentEnd, e.timestamp())
	{};

	virtual ~HExperimentEndEvent() {};

	QString eventInfo() const;
	//virtual QDataStream& putAdditional(QDataStream& stream) const;
	static HExperimentEndEvent* getEvent(QDataStream& stream, int timestamp);
	//virtual QString eventCSVAdditional() const;


	HEvent* clone(int ts = 0) const
	{
		int t = (ts < 0 ? timestamp() : ts);
		return new HExperimentEndEvent(t);
	};

private:
};


class HReliabilityStartEvent: public HEvent
{
public:
	HReliabilityStartEvent(int timestamp)
	: HEvent(HEventType::HEventReliabilityStart, timestamp)
	{};

	HReliabilityStartEvent(const HReliabilityStartEvent& e)
	: HEvent(HEventType::HEventReliabilityStart, e.timestamp())
	{};

	virtual ~HReliabilityStartEvent() {};

	QString eventInfo() const;
	//virtual QDataStream& putAdditional(QDataStream& stream) const;
	static HReliabilityStartEvent* getEvent(QDataStream& stream, int timestamp);
	//virtual QString eventCSVAdditional() const;

	HEvent* clone(int ts = 0) const
	{
		int t = (ts < 0 ? timestamp() : ts);
		return new HReliabilityStartEvent(t);
	};

private:
};

class HReliabilityEndEvent: public HEvent
{
public:
	HReliabilityEndEvent(int timestamp)
	: HEvent(HEventType::HEventReliabilityEnd, timestamp)
	{};

	HReliabilityEndEvent(const HReliabilityEndEvent& e)
	: HEvent(HEventType::HEventReliabilityEnd, e.timestamp())
	{};

	virtual ~HReliabilityEndEvent() {};

	QString eventInfo() const;
	//virtual QDataStream& putAdditional(QDataStream& stream) const;
	static HReliabilityEndEvent* getEvent(QDataStream& stream, int timestamp);
	//virtual QString eventCSVAdditional() const;

	HEvent* clone(int ts = 0) const
	{
		int t = (ts < 0 ? timestamp() : ts);
		return new HReliabilityEndEvent(t);
	};

private:
};


class HLookEnabledEvent: public HEvent
{
public:
	HLookEnabledEvent(int timestamp)
	: HEvent(HEventType::HEventLookEnabled, timestamp)
	{};

	HLookEnabledEvent(const HLookEnabledEvent& e)
	: HEvent(HEventType::HEventLookEnabled, e.timestamp())
	{};

	virtual ~HLookEnabledEvent() {};

	QString eventInfo() const;
	//virtual QDataStream& putAdditional(QDataStream& stream) const;
	static HLookEnabledEvent* getEvent(QDataStream& stream, int timestamp);
	//virtual QString eventCSVAdditional() const;

	HEvent* clone(int ts = 0) const
	{
		int t = (ts < 0 ? timestamp() : ts);
		return new HLookEnabledEvent(t);
	};

private:
};


class HAGLookEnabledEvent: public HEvent
{
public:
	HAGLookEnabledEvent(int timestamp)
	: HEvent(HEventType::HEventAGLookEnabled, timestamp)
	{};

	HAGLookEnabledEvent(const HAGLookEnabledEvent& e)
	: HEvent(HEventType::HEventAGLookEnabled, e.timestamp())
	{};

	virtual ~HAGLookEnabledEvent() {};

	QString eventInfo() const;
	//virtual QDataStream& putAdditional(QDataStream& stream) const;
	static HAGLookEnabledEvent* getEvent(QDataStream& stream, int timestamp);
	//virtual QString eventCSVAdditional() const;

	HEvent* clone(int ts = 0) const
	{
		int t = (ts < 0 ? timestamp() : ts);
		return new HAGLookEnabledEvent(t);
	};

private:
};


class HLookDisabledEvent: public HEvent
{
public:
	HLookDisabledEvent(int timestamp)
	: HEvent(HEventType::HEventLookDisabled, timestamp)
	{};

	HLookDisabledEvent(const HLookDisabledEvent& e)
	: HEvent(HEventType::HEventLookDisabled, e.timestamp())
	{};

	virtual ~HLookDisabledEvent() {};

	QString eventInfo() const;
	//virtual QDataStream& putAdditional(QDataStream& stream) const;
	static HLookDisabledEvent* getEvent(QDataStream& stream, int timestamp);
	//virtual QString eventCSVAdditional() const;

	HEvent* clone(int ts = 0) const
	{
		int t = (ts < 0 ? timestamp() : ts);
		return new HLookDisabledEvent(t);
	};

private:
};

class HScreenStartEvent: public HEvent
{
public:
	HScreenStartEvent(const QString& filename, int playerid=-1, int timestamp=0)
	: HEvent(HEventType::HEventScreenStart, timestamp)
	, m_filename(filename)
	, m_playerid(playerid)
	{};

	HScreenStartEvent(const HScreenStartEvent& e)
	: HEvent(HEventType::HEventScreenStart, e.timestamp())
	, m_filename(e.filename())
	, m_playerid(e.playerid())
	{};

	virtual ~HScreenStartEvent() {};

	QString eventInfo() const;
	virtual QString eventCSVAdditional() const;
	int playerid() const { return m_playerid; };
	const QString& filename() const { return m_filename; };

	virtual QDataStream& putAdditional(QDataStream& stream) const;
	static HScreenStartEvent* getEvent(QDataStream& stream, int timestamp);

	HEvent* clone(int ts = 0) const
	{
		int t = (ts < 0 ? timestamp() : ts);
		return new HScreenStartEvent(filename(), playerid(), t);
	};

private:
	QString m_filename;
	int m_playerid;
};


#endif
