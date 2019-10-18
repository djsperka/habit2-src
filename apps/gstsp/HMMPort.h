/*
 * HMMPort.h
 *
 *  Created on: Oct 16, 2019
 *      Author: dan
 */

#ifndef HMMPORT_H_
#define HMMPORT_H_

#include "HMMTypes.h"
#include <gst/gst.h>
#include <map>
#include <list>

class HMMStim;

class HMMPort
{
	typedef std::map<HMMStimPosition, GstElement *> HMMPortPosEleMap;
	HMMPortPosEleMap m_mapPosEle;				// video only; StimPosition <--> GstElement (downstream)
	GstElement *m_mixer;						// the actual mixer used in pipeline
	std::list<GstPad *> m_listRequestPads;		// request pads taken from audio mixer

public:
	HMMPort();
	virtual ~HMMPort();

	void addVideoEle(HMMStimPosition pos, GstElement *ele);
	void addAudioMixer(GstElement *mixer);

	void connect(HMMStim& stim);	// assumes not connected, will throw otherwise
	void disconnect();				// might not be connected, will be OK if not
};

#endif /* HMMPORT_H_ */
