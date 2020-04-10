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


// Attach this to BOTH pad-added callback AND blocking probe. Initialize with value 1.
// Increment in pad-added, decrement in no-more-pads and in each blocking probe. When this
// counter hits zero, we know that all streams have been found&started by the decoder, and
// each of the streams is now blocked.
// If this is an image, where there isn't a stream - just a single image passed (needs imagefreeze later) -
// that's all we need. If its a video or audio stream, then we set up a flushing seek, and
// reset the counter to the number of streams. We post a bus message, and the bus callback issues a
// flushing seek. In blocking probe we decrement again, and when this fires we are
// prerolled.



/*
 *
 *
 *
 *     BXCVADFGHJKL1234567890
 *
 *
 *
 *
 */





class SourcePrerollCounter: public Counter
{
	Source *m_psource;
	GstElement *m_pipeline;
	bool m_bIsImage;	// if true, no flushing seek needed to preroll.
public:
	SourcePrerollCounter(Source *psource, GstElement *pipeline, int counter=0, Counter *psubCounter=NULL): Counter(counter, psubCounter), m_psource(psource), m_pipeline(pipeline), m_bIsImage(false) {}
	void operator()(void);
	Source *source() const { return m_psource; }
	GstElement *pipeline() const { return m_pipeline; }
	void setIsImage(bool isImage) { m_bIsImage = isImage; }
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
	HMMInstanceID m_iid;
	HMM* m_phmm;
public:
	PlayStimCounter(HMMInstanceID iid, HMM* phmm, int counter=0): Counter(counter), m_iid(iid), m_phmm(phmm) {}
	~PlayStimCounter() {}
	HMMInstanceID iid() { return m_iid; }
	HMM* hmm() { return m_phmm; }
	void operator()(void);
};


};	// end namespace


#endif /* HMMCOUNTER_H_ */
