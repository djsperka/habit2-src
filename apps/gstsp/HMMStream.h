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



#endif /* HMMSTREAM_H_ */
