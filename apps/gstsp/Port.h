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

namespace hmm {

class Stim;

class Port
{
	typedef std::map<HMMStimPosition, GstElement *> HMMPortPosEleMap;
	HMMPortPosEleMap m_mapPosEle;				// video only; StimPosition <--> GstElement (downstream)
	GstElement *m_mixer;						// the actual mixer used in pipeline
	std::list<GstPad *> m_listRequestPads;		// request pads taken from audio mixer

public:
	Port();
	virtual ~Port();

	void addVideoEle(HMMStimPosition pos, GstElement *ele);
	void addAudioMixer(GstElement *mixer);

	void connect(Stim& stim);	// assumes not connected, will throw otherwise
	void disconnect();				// might not be connected, will be OK if not
};

}
#endif /* HMMPORT_H_ */
