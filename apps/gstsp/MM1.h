/*
 * MM1.h
 *
 *  Created on: Sep 6, 2019
 *      Author: dan
 */

#ifndef MM1_H_
#define MM1_H_

#include <string>
#include <gst/gst.h>
#include <vector>
#include <list>
#include <boost/ptr_container/ptr_list.hpp>
#include <boost/ptr_container/ptr_map.hpp>

enum class MMSourceType
{
	NONE,
	VIDEO_ONLY,
	AUDIO_ONLY,
	AUDIO_VIDEO,
};

enum class MMStreamType
{
	NONE,
	VIDEO,
	AUDIO
};

enum class MMStimState
{
	NONE,
	PREROLLING,
	PREROLLED,
	PLAYING,
	PENDING_PAUSE,
	PAUSED
};

typedef int MMStimPosition;

// MMStream corresponds to a single stream (audio or video) for a single source (e.g. a file).
struct MMStream
{
	bool isVideo;	// if false, its audio
	GstPad *srcpad;
	gulong probeid;
};

// MMSource represents a single source (e.g. a file), which may produce one or more streams, but
// no more than one each of audio, video.
// The source type represents what the configuration expects this file to contain, even though the
// file may not have those pads. Source type can be VIDEO_ONLY, AUDIO_ONLY, AUDIO_VIDEO, and the value
// tells MMSource how to handle video and/or audio streams from the source. A source may contain a stream
// that is ignored here - e.g. its OK to specify AUDIO_ONLY on a movie file that contains video and audio,
// the video stream is safely ignored.

class MMSource
{
	int counter;	// used to track async actions on all streams
	MMStimState stimState;
public:
	MMSource() {};
	virtual ~MMSource() {};
	MMSource(const MMSource&) = delete;
	void operator=(const MMSource&) = delete;
	bool dec_counter();
	void inc_counter();
	void set_counter(int c);
	void set_stim_state(MMStimState state) { stimState = state; };
	MMStimState state() const { return stimState; };
	void addStream(MMStream *pstream);	// takes ownership
	MMSourceType sourceType;
	bool prerolled;
	GstElement *ele;	// a bin, but here probably uridecodebin
	boost::ptr_list<MMStream> streams;
	void *pstim;	// bad bad bad
};

// MMStim would correspond to StimulusSettings - the collection of individual sources (files)
// that comprise the "stimulus" to be presented. The MMStimPosition corresponds to the
// PlayerPositionType, L/R/C/ISS, and more generally provides a video stream on the src pad if
// ppt is a video type, and audio on the src pad is an audio type. Its up the the application
// to decide what to do with all of them.

class MMStim
{
	std::string filename;
	GstElement *m_pipeline;
	boost::ptr_map<MMStimPosition, MMSource> m_sourcemap;
	int counter;
	bool prerolled;
	MMStimState stimState;
public:
	MMStim(const std::string& f, GstElement *pipeline) : filename(f), m_pipeline(pipeline), counter(0), prerolled(false), stimState(MMStimState::NONE) {};
	~MMStim() {};
	std::string uri() const;
	void addSource(MMStimPosition, MMSource* psrc);
	void set_counter(int c);
	bool dec_counter();
	bool isPrerolled() const {return prerolled; };
	void set_stim_state(MMStimState state) { stimState = state; };
	MMStimState state() const { return stimState; };
	const boost::ptr_map<MMStimPosition, MMSource>& sourcemap() const { return m_sourcemap; };
};

class MM1
{
	bool m_bVideo, m_bAudio;
	GstElement *m_pipeline;
	GstElement *m_bkgd;
	GstElement *m_conv, *m_scale, *m_vsink;	// pipeline video goes through in order conv-scale-vsink
	boost::ptr_map<unsigned long, MMStim> stimMap;

//	typedef struct
//	{
//		GstPad *srcpad;
//		gulong probeid;
//	} PadStruct;
//
//	typedef struct
//	{
//		std::string filename;
//		GstElement *filesrc, *dbin;
//	} SourceStruct;

	unsigned long m_ulCurrentStim;
	unsigned long m_ulBkgd;
	unsigned long m_ulPending;

	std::string getUri(const std::string& filename);

public:
	MM1(bool bVideo, bool bAudio=false, const std::string& bkgd="videotestsrc");
	virtual ~MM1();


	// add a (single) file as a source
	unsigned int addStim(const std::string& filename);
	unsigned int addStim(MMStim* pstim);
	void preroll(unsigned long id);
	void play(unsigned long id);

	bool hasStim(unsigned long id);
	const MMStim& getStim(unsigned long id);	// throws, better check
	unsigned long getPendingID() const { return m_ulPending; };

	// bus callback - bus messages here
	static gboolean busCallback(GstBus *, GstMessage *msg, gpointer pdata);

	static void padAddedCallback(GstElement * element, GstPad * pad, MMSource *pmmsrc);
	static void noMorePadsCallback(GstElement * element, MMSource *pmmsrc);
	static GstPadProbeReturn padProbeBlockCallback (GstPad * pad, GstPadProbeInfo * info, gpointer user_data);


};




#endif /* MM1_H_ */
