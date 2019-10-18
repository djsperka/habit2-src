/*
 * HMMSource.h
 *
 *  Created on: Oct 17, 2019
 *      Author: dan
 */

#ifndef HMMSOURCE_H_
#define HMMSOURCE_H_

#include "HMMStream.h"
#include <memory>
#include <map>

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





#endif /* HMMSOURCE_H_ */
