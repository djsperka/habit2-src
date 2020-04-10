/*
 * HMMStream.h
 *
 *  Created on: Oct 17, 2019
 *      Author: dan
 */

#ifndef HMMSTREAM_H_
#define HMMSTREAM_H_

#include "HMMTypes.h"
#include <gst/gst.h>

// HMMStream corresponds to a single stream (audio or video) for a single source (e.g. a file).
namespace hmm {
class Source;
class Stream
{
	GstPad *m_srcpad;
	Source *m_parent;
	GstElement *m_sink;
	gulong m_probeid;	// this can be a blocking probe or an idle probe. If its nonzero, its blocking m_srcpad.
	gulong m_probeidEvent; // event probe handy for looping

	Stream(GstPad *src, Source *parent, GstElement *sink, gulong probeid=0): m_srcpad(src), m_parent(parent), m_sink(sink), m_probeid(probeid), m_probeidEvent(0) {}

public:
	virtual ~Stream() { gst_object_unref(m_srcpad); }	// NOTE not unref-ing sink!
	GstPad* srcpad() { return m_srcpad; }
	//GstElement *sink() { return m_sink; }
	//void setSink(GstElement *sink) { m_sink = sink; }
	void setProbeID(gulong probeid) { m_probeid = probeid; }
	gulong getProbeID() { return m_probeid; }
	void setEventProbeID(gulong probeid) { m_probeidEvent = probeid; }
	gulong getEventProbeID() { return m_probeidEvent; }
	Source *parentSource() { return m_parent; }

	friend class Source;
};
};	// end namespace hmm


#endif /* HMMSTREAM_H_ */
