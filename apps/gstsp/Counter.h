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

namespace hmm {

class Stim;
class Source;

class Counter
{
	int m_counter;
	Counter *m_psubCounter;
public:
	Counter(int counter=0, Counter *psub=nullptr): m_counter(counter), m_psubCounter(psub) {};
	virtual ~Counter() {};
	void setCounter(int c) { m_counter = c; };
	bool decrement();
	void increment();
	virtual void operator()(void) = 0;		// action to be taken
};


class StimPrerollCounter: public Counter
{
	Stim *m_pstim;
public:
	StimPrerollCounter(Stim *pstim, int counter=0): Counter(counter), m_pstim(pstim) {}
	void operator()(void);
	Stim *stim() const { return m_pstim; }
};

class SourcePrerollCounter: public Counter
{
	Source *m_psource;
	GstElement *m_pipeline;
public:
	SourcePrerollCounter(Source *psource, GstElement *pipeline, int counter=0, Counter *psubCounter=NULL): Counter(counter, psubCounter), m_psource(psource), m_pipeline(pipeline) {}
	void operator()(void);
	Source *source() const { return m_psource; }
	GstElement *pipeline() const { return m_pipeline; }
};

// can use this on sources to carry along another counter to act on stim.
class NoopCounter: public Counter
{
public:
	NoopCounter(int counter=0, Counter *psubCounter=NULL): Counter(counter, psubCounter) {}
	void operator()(void);
};

class HMM;
class StimSwapCounter: public Counter
{
	HMMStimID m_stimidCurrent;
	HMMStimID m_stimidPending;
	HMM* m_pmm;
public:
	StimSwapCounter(HMMStimID curr, HMMStimID pend, HMM* pmm, int counter=0): Counter(counter), m_stimidCurrent(curr), m_stimidPending(pend), m_pmm(pmm) {}
	void operator()(void);
	HMMStimID current() const { return m_stimidCurrent; }
	HMMStimID pending() const { return m_stimidPending; }
	HMM* hmm() { return m_pmm; }
};


class PlayStimCounter: public Counter
{
	Stim *m_pCurrent;
	Stim *m_pPending;
	HMM* m_phmm;
public:
	PlayStimCounter(Stim *current, Stim *pending, HMM* phmm, int counter=0): Counter(counter), m_pCurrent(current), m_pPending(pending), m_phmm(phmm) {}
	~PlayStimCounter() {}
	Stim *current() { return m_pCurrent; }
	Stim *pending() { return m_pPending; }
	HMM* hmm() { return m_phmm; }
	void operator()(void);
};


};	// end namespace


#endif /* HMMCOUNTER_H_ */