/*
 * HMMSource.h
 *
 *  Created on: Oct 17, 2019
 *      Author: dan
 */

#ifndef HMMSOURCE_H_
#define HMMSOURCE_H_

#include "Stream.h"
#include "Counter.h"
#include <memory>
#include <map>
#include <string>

// MMSource represents a single source (e.g. a file), which may produce one or more streams, but
// no more than one each of audio, video.
// The source type represents what the configuration expects this file to contain, even though the
// file may not have those pads. Source type can be VIDEO_ONLY, AUDIO_ONLY, AUDIO_VIDEO, and the value
// tells MMSource how to handle video and/or audio streams from the source. A source may contain a stream
// that is ignored here - e.g. its OK to specify AUDIO_ONLY on a movie file that contains video and audio,
// the video stream is safely ignored.

namespace hmm
{
class Stim;
class Source
{
public:
	//typedef std::unique_ptr<Stream> stream_ptr;
	typedef Stream* StreamP;
private:
	HMMSourceType m_sourceType;	// what types of stream will be accepted in padAdded
	Stim *m_parent;
	std::map<HMMStreamType, StreamP> m_streamMap;
	bool m_bSeeking;

protected:
	Source(HMMSourceType t, Stim *parent);

public:
	Source(const Source&) = delete;
	virtual ~Source() {};
	void operator=(const Source&) = delete;
	HMMSourceType sourceType() const { return m_sourceType; }
	std::map<HMMStreamType, StreamP>& streamMap() { return m_streamMap; }
	void addStream(HMMStreamType streamType, GstPad *srcpad, GstElement *sink, gulong probeid=0);
	Stream *getStream(HMMStreamType t);
	Stim *parentStim() { return m_parent; }
	void setSeeking(bool b) { m_bSeeking = b; }
	bool isSeeking() const { return m_bSeeking; }


	static void parseCaps(GstCaps* caps, bool& isVideo, bool& isImage, int& width, int& height, bool& isAudio);

	virtual GstElement *bin() = 0;	// element for seeks on this source
	virtual void preroll(GstElement *pipeline, Counter *pc) = 0;
	virtual void stop() = 0;
	virtual void rewind() = 0;
};

class ColorSource: public Source
{
	ColorSource(HMMSourceType, Stim *parent, unsigned int argb);
	unsigned int m_argb;
	GstElement *m_ele;
public:
	GstElement *bin();
	virtual void preroll(GstElement *pipeline, Counter *pc);
	virtual void stop();
	virtual void rewind();
	friend class Stim;
};

class FileSource: public Source
{
	std::string m_filename;
	bool m_bloop;
	unsigned int m_volume;
	GstElement *m_ele;
	FileSource(HMMSourceType t, Stim *parent, const std::string& file, bool bloop, unsigned int volume);

public:
	GstElement *bin();
	virtual void preroll(GstElement *pipeline, Counter *pc);
	virtual void stop();
	virtual void rewind();
	friend class Stim;

	static void noMorePadsCallback(GstElement *, SourcePrerollCounter *pspc);
	static void padAddedCallback(GstElement *, GstPad * pad, SourcePrerollCounter *pspc);
	static GstPadProbeReturn padProbeBlockCallback(GstPad *, GstPadProbeInfo *, gpointer user_data);
};
}




#endif /* HMMSOURCE_H_ */
