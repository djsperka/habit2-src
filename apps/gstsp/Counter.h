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
class FileSource;
class ImageSource;
class ColorSource;

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

class ColorSourcePrerollCounter: public Counter
{
	ColorSource *m_psource;
public:
	ColorSourcePrerollCounter(ColorSource *psource, int counter=0, Counter *psubCounter=NULL): Counter(counter, psubCounter), m_psource(psource) {}
	virtual ~ColorSourcePrerollCounter() {};
	void operator()(void);
	ColorSource *colorSource() const { return m_psource; }
};


class ImageSourcePrerollCounter: public Counter
{
	ImageSource *m_psource;
public:
	ImageSourcePrerollCounter(ImageSource *psource, int counter=0, Counter *psubCounter=NULL): Counter(counter, psubCounter), m_psource(psource) {}
	virtual ~ImageSourcePrerollCounter() {};
	void operator()(void);
	ImageSource *imageSource() const { return m_psource; }
};



class FileSourcePrerollCounter: public Counter
{
	FileSource *m_psource;
	GstElement *m_pipeline;
	bool m_bIsImage;	// if true, no flushing seek needed to preroll.
public:
	FileSourcePrerollCounter(FileSource *psource, GstElement *pipeline, int counter=0, Counter *psubCounter=NULL): Counter(counter, psubCounter), m_psource(psource), m_pipeline(pipeline), m_bIsImage(false) {}
	virtual ~FileSourcePrerollCounter() {}
	void operator()(void);
	FileSource *fileSource() const { return m_psource; }
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
	HMMInstanceID m_iid;
	HMM* m_phmm;
public:
	StimSwapCounter(HMMInstanceID iid, HMM* phmm, int counter=0): Counter(counter), m_iid(iid), m_phmm(phmm) {}
	virtual ~StimSwapCounter() {}
	HMMInstanceID iid() { return m_iid; }
	HMM* hmm() { return m_phmm; }
	void operator()(void);
};

class StimPlayCounter: public Counter
{
	HMMInstanceID m_iid;
	HMM *m_phmm;
	bool m_bPrerolled;	// state
public:
	StimPlayCounter(HMMInstanceID iid, HMM *phmm, int counter=0): Counter(counter), m_iid(iid), m_phmm(phmm), m_bPrerolled(false) {}
	virtual ~StimPlayCounter() {};
	HMMInstanceID iid() { return m_iid; }
	HMM* hmm() { return m_phmm; }
	void operator()(void);
};

};	// end namespace


#endif /* HMMCOUNTER_H_ */
