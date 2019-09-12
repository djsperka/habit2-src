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
#include <map>


enum class MMSrcType
{
	NONE,
	VIDEO,
	AUDIO
};

// MMStream corresponds to a single stream (audio or video) for a single source (e.g. a file).
struct MMStream
{
	bool isVideo;	// if false, its audio
	GstPad *srcpad;
	gulong probeid;
};

// MMSource represents a single source (e.g. a file), which may produce one or more streams, but
// no more than one each of audio, video.
struct MMSource
{
	int counter;	// used to track async actions on all streams
	bool prerolled;
	GstElement *ele;	// a bin, but here probably uridecodebin
	std::list<MMStream>;
};

// MMStim would correspond to StimulusSettings - the collection of individual sources (files)
// that comprise the "stimulus" to be presented. The MMSrcType enum corresponds to the
// PlayerPositionType, which distinguishes the various sources from one another.
class MMStim
{
	std::map<MMSrcType, MMSource *>;
};


class MM1
{
	bool m_bVideo, m_bAudio;
	GstElement *m_pipeline;
	GstElement *m_bkgd;
	GstElement *m_conv, *m_scale, *m_vsink;	// pipeline video goes through in order conv-scale-vsink

	typedef struct
	{
		GstPad *srcpad;
		gulong probeid;
	} PadStruct;

	typedef struct
	{
		std::string filename;
		GstElement *filesrc, *dbin;
	} SourceStruct;

	std::vector<SourceStruct> m_sources;
	unsigned int m_uiCurrentSource;

public:
	MM1(bool bVideo, bool bAudio=false, const std::string& bkgd="videotestsrc");
	virtual ~MM1();


	// add a (single) file as a source
	unsigned int addSource(const std::string& filename);
	bool preroll(unsigned int id);

	// bus callback - bus messages here
	static gboolean busCallback(GstBus *, GstMessage *msg, gpointer pdata);

};

#endif /* MM1_H_ */
