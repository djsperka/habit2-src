/*
 * HMM.h
 *
 *  Created on: Sep 17, 2019
 *      Author: dan
 */

#ifndef HMM_H_
#define HMM_H_

#include <map>
#include <memory>
#include <list>
#include <string>
#include <gst/gst.h>

enum class HMMSourceType
{
	NONE,
	VIDEO_ONLY,
	AUDIO_ONLY,
	AUDIO_VIDEO,
};

enum class HMMStreamType
{
	NONE,
	VIDEO,
	AUDIO
};

enum class HMMStimState
{
	NONE,
	PREROLLING,
	PREROLLED,
	PLAYING,
	PENDING_PAUSE,
	PAUSED
};

typedef int HMMStimPosition;
typedef unsigned long HMMStimID;


// HMMStream corresponds to a single stream (audio or video) for a single source (e.g. a file).
class HMMStream
{
	GstPad *m_srcpad;
	gulong m_probeid;	// this can be a blocking probe or an idle probe. If its nonzero, its blocking m_srcpad.
	gulong m_probeidEvent; // event probe handy for looping
public:
	HMMStream(GstPad *src, gulong probeid=0): m_srcpad(src), m_probeid(probeid), m_probeidEvent(0) {}
	GstPad* srcpad() { return m_srcpad; }
	void setProbeID(gulong probeid) { m_probeid = probeid; }
	gulong getProbeID() { return m_probeid; }
	void setEventProbeID(gulong probeid) { m_probeidEvent = probeid; }
	gulong getEventProbeID() { return m_probeidEvent; }
};


// MMSource represents a single source (e.g. a file), which may produce one or more streams, but
// no more than one each of audio, video.
// The source type represents what the configuration expects this file to contain, even though the
// file may not have those pads. Source type can be VIDEO_ONLY, AUDIO_ONLY, AUDIO_VIDEO, and the value
// tells MMSource how to handle video and/or audio streams from the source. A source may contain a stream
// that is ignored here - e.g. its OK to specify AUDIO_ONLY on a movie file that contains video and audio,
// the video stream is safely ignored.

class HMMSource
{
public:
	typedef std::unique_ptr<HMMStream> stream_ptr;
private:
	HMMSourceType m_sourceType;	// what types of stream will be accepted in padAdded
	GstElement *m_bin;			// element for seeks on this source
	std::map<HMMStreamType, stream_ptr> m_streamMap;
	bool m_bloop;

public:
	HMMSource(HMMSourceType t, GstElement *bin, bool loop=false);
	HMMSource(const HMMSource&) = delete;
	virtual ~HMMSource() {};
	void operator=(const HMMSource&) = delete;
	void addStream(HMMStreamType t, HMMStream *pstream);	// takes ownership
	HMMSourceType sourceType() const { return m_sourceType; }
	std::map<HMMStreamType, stream_ptr>& streamMap() { return m_streamMap; }
	HMMStream *getStream(HMMStreamType t);
	GstElement *bin() { return m_bin; }
	void setLooping(bool b) { m_bloop = b; }
	bool isLooping() { return m_bloop; }
};


class HMMCounter
{
	int m_counter;
	HMMCounter *m_psubCounter;
public:
	HMMCounter(int counter=0, HMMCounter *psub=NULL): m_counter(counter), m_psubCounter(psub) {};
	virtual ~HMMCounter() {};
	void setCounter(int c) { m_counter = c; };
	bool decrement();
	void increment();
	virtual void operator()(void) = 0;		// action to be taken
};


class HMMStim;
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

// HMMStim would correspond to StimulusSettings - the collection of individual sources (files)
// that comprise the "stimulus" to be presented. The MMStimPosition corresponds to the
// PlayerPositionType, L/R/C/ISS, and more generally provides a video stream on the src pad if
// ppt is a video type, and audio on the src pad is an audio type. Its up the the application
// to decide what to do with all of them.

class HMMStim
{
public:
	typedef std::unique_ptr<HMMSource> source_ptr;
private:
	HMMStimState m_stimState;
	std::map<HMMStimPosition, source_ptr> m_sourceMap;
public:
	HMMStim(): m_stimState(HMMStimState::NONE) {};
	virtual ~HMMStim() {};
	void addSource(HMMStimPosition, HMMSource* psrc);
	HMMSource *getSource(HMMStimPosition pos);
	void setStimState(HMMStimState state) { m_stimState = state; }
	HMMStimState getStimState() const { return m_stimState; }
	std::map<HMMStimPosition, source_ptr>& sourceMap() { return m_sourceMap; }
};


struct HMMVideoTail
{
	GstElement *m_conv, *m_scale, *m_vsink;
};

class HMM
{
	bool m_bVideo, m_bAudio;
	GstElement *m_pipeline;

	// the stim info is supplied when addStim is called - it represents _the_ability_to_ display/play that "stim"
	// What does a stim contain?
	//   The HMM would be created with a specification for what stream(s) is(are) expected from each stim.
	//   The HMM will map audio output to the <HOW DOES GSTREAMER CHOOSE AUDIO OUTPUT>
	//   The specification can be for 0-n video sinks, all in "gstqtvideosink" (NOT IMPLEMENTED - BELOW IS SINGLE-VIDEO-ONLY)
	//   The application must assign the widgets to sinks.
	// To use, first create HMM with spec, assign widgets, show() them, you should see bkgd.
	// addStim() as much as you like, save ids returned.
	// preroll stim first checks state of stim (NOT IMPLEMENTED) - is it prerolled, playing, or uninitialized? Will init
	// the stim and add to m_stimMap if needed. If playing, s/b exception?

	// stim_info corresponds to StimulusSettings
	// a factory method will be used to create the HMMStim from this object's description

	typedef std::pair<bool, std::string> stim_info;	// first: true->isFile, false->is a bin description, e.g. "videotestsrc"
	std::map<HMMStimID, stim_info> m_stimInfoMap;

	// this is where prerolled stim and playing stim are. When replaced, dispose of HMMStim (TODO - persistence of ag)

	typedef std::unique_ptr<HMMStim> stim_ptr;
	std::map<HMMStimID, stim_ptr> m_stimMap;

	HMMStimID m_stimidCurrent;
	HMMStimID m_stimidBkgd;
	//HMMStimID m_stimidPending;

	std::map<HMMStimPosition, HMMVideoTail> m_stimTailMap;

	std::string getUri(const std::string& filename);

	// substitute stim with 'id' for current stim, make it current (return previous stimid)
	HMMStimID swap(HMMStimID id);

	// factory
	HMMStim* makeStimFromFile(HMMStimID id, const std::string& filename);
	HMMStim* makeStimFromDesc(HMMStimID id, const std::string& description);

	// for in-use counters
	std::map<HMMStimID, HMMCounter *> m_mapCounters;

public:
	HMM(const std::string& bkgd=std::string("videotestsrc"));
	virtual ~HMM();


	// add a (single) file as a source
	HMMStimID addStimInfo(const std::string& filename_or_description, bool bIsFile=true);
	HMMStim *getStim(HMMStimID id);
	void preroll(HMMStimID id);
	void play(HMMStimID id);
	void dump(const char *c);

	// tail
	void addVideoTail(HMMStimPosition pos, HMMVideoTail tail) { m_stimTailMap[pos] = tail; }
	HMMVideoTail* getVideoTail(HMMStimPosition pos);

	// get stuff
	GstElement *pipeline() { return m_pipeline; }

	// bus callback - bus messages here
	static gboolean busCallback(GstBus *, GstMessage *msg, gpointer pdata);
	static void padAddedCallback(GstElement * element, GstPad * pad, HMMSourcePrerollCounter *pcounter);
	static void noMorePadsCallback(GstElement * element, HMMSourcePrerollCounter *pcounter);
	static GstPadProbeReturn padProbeBlockCallback(GstPad * pad, GstPadProbeInfo * info, gpointer user_data);
	static GstPadProbeReturn eventProbeCallback(GstPad * pad, GstPadProbeInfo * info, gpointer p);
	static GstPadProbeReturn padProbeIdleCallback(GstPad *, GstPadProbeInfo *, gpointer user_data);

};

#endif /* HMM_H_ */
