/*
 * HMMCounter.h
 *
 *  Created on: Oct 17, 2019
 *      Author: dan
 */

#ifndef HMMCOUNTER_H_
#define HMMCOUNTER_H_

#include "HMMTypes.h"
#include <gst/gst.h>

class HMMStim;
class HMMSource;

class HMMCounter
{
	int m_counter;
	HMMCounter *m_psubCounter;
public:
	HMMCounter(int counter=0, HMMCounter *psub=nullptr): m_counter(counter), m_psubCounter(psub) {};
	virtual ~HMMCounter() {};
	void setCounter(int c) { m_counter = c; };
	bool decrement();
	void increment();
	virtual void operator()(void) = 0;		// action to be taken
};


class HMMStimPrerollCounter: public HMMCounter
{
	HMMStimID m_id;
	HMMStim *m_pstim;
public:
	HMMStimPrerollCounter(HMMStimID id, HMMStim *pstim, int counter=0): HMMCounter(counter), m_id(id), m_pstim(pstim) {}
	void operator()(void);
	HMMStim *stim() const { return m_pstim; }
	HMMStimID id() const {return m_id; }
};

class HMMSourcePrerollCounter: public HMMCounter
{
	HMMStimID m_id;
	HMMStimPosition  m_pos;
	HMMSource *m_psource;
	GstElement *m_pipeline;
public:
	HMMSourcePrerollCounter(HMMStimID id, HMMStimPosition pos, HMMSource *psource, GstElement *pipeline, int counter=0, HMMCounter *psubCounter=NULL): HMMCounter(counter, psubCounter), m_id(id), m_pos(pos), m_psource(psource), m_pipeline(pipeline) {}
	void operator()(void);
	HMMSource *source() const { return m_psource; }
	HMMStimID id() const {return m_id; }
	GstElement *pipeline() const { return m_pipeline; }
	HMMStimPosition pos() const { return m_pos; }
};

// can use this on sources to carry along another counter to act on stim.
class HMMNoopCounter: public HMMCounter
{
public:
	HMMNoopCounter(int counter=0, HMMCounter *psubCounter=NULL): HMMCounter(counter, psubCounter) {}
	void operator()(void);
};

class HMM;
class HMMStimSwapCounter: public HMMCounter
{
	HMMStimID m_stimidCurrent;
	HMMStimID m_stimidPending;
	HMM* m_pmm;
public:
	HMMStimSwapCounter(HMMStimID curr, HMMStimID pend, HMM* pmm, int counter=0): HMMCounter(counter), m_stimidCurrent(curr), m_stimidPending(pend), m_pmm(pmm) {}
	void operator()(void);
	HMMStimID current() const { return m_stimidCurrent; }
	HMMStimID pending() const { return m_stimidPending; }
	HMM* hmm() { return m_pmm; }
};


class HMMPlayStimCounter: public HMMCounter
{
	HMMStimID m_stimidCurrent;
	HMMStimID m_stimidPending;
	HMM* m_phmm;
public:
	HMMPlayStimCounter(HMMStimID current, HMMStimID pending, HMM* phmm, int counter=0): HMMCounter(counter), m_stimidCurrent(current), m_stimidPending(pending), m_phmm(phmm) {}
	~HMMPlayStimCounter() {}
	HMMStimID current() { return m_stimidCurrent; }
	HMMStimID pending() { return m_stimidPending; }
	HMM* hmm() { return m_phmm; }
	void operator()(void);
};





#endif /* HMMCOUNTER_H_ */
