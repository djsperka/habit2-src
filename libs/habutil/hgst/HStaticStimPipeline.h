/*
 * HAGStimPipeline.h
 *
 *  Created on: Dec 26, 2017
 *      Author: dan
 */

#ifndef LIBS_HABUTIL_HGST_HSTATICSTIMPIPELINE_H_
#define LIBS_HABUTIL_HGST_HSTATICSTIMPIPELINE_H_

#include <hgst/HStimPipeline.h>

class HStaticStimPipeline: public HStimPipeline
{
	Q_OBJECT

public:
	HStaticStimPipeline(int id, const Habit::StimulusSettings& stimulusSettings, const QDir& stimRoot, const HStimulusLayoutType& layoutType, bool bISS, QObject *parent);
	virtual ~HStaticStimPipeline();

	// The only reason for this class is to have a no-op cleanup() function. That way HGMM::playStim can safely call cleanup() on every stim
	// it disconnects and the AG (and any other stim that should not be cleaned up) will be safe.
	virtual void cleanup();

	virtual void forceCleanup();
};

#endif /* LIBS_HABUTIL_HGST_HSTATICSTIMPIPELINE_H_ */