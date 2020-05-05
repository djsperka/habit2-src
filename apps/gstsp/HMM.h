/*
 * HMM.h
 *
 *  Created on: Sep 17, 2019
 *      Author: dan
 */

#ifndef HMM_H_
#define HMM_H_

#include "HMMTypes.h"
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <gst/gst.h>
#include "stimulussettings.h"
#include "Stream.h"
#include "Source.h"
#include "Stim.h"
#include "Counter.h"
#include "Port.h"
#include "StimFactory.h"



namespace hmm {

struct HMMVideoTail
{
	GstElement *m_conv, *m_scale, *m_vsink;
};

typedef std::map<HMMStimPosition, std::string> StimPosTailMap;
struct HMMConfiguration
{
	StimPosTailMap video;
	StimPosTailMap audio;
	void addVideoSink(HMMStimPosition pos, const std::string& sink = std::string("qwidget5videosink"));
	void addAudioSink(HMMStimPosition pos, const std::string& sink = std::string());	// empty string means "figure it out" depending on platform
};

class HMM
{
	bool m_bVideo, m_bAudio;
	GstElement *m_pipeline;
	GThread *m_gthread;
	GMainLoop *m_pgml;
	StimFactory& m_factory;
	GstElement *m_cameraBin;

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

	//std::map<HMMStimID, Habit::StimulusSettings> m_ssMap;
	typedef Stim* StimP;
	typedef std::map<HMMInstanceID, StimP> InstanceMap;
	InstanceMap m_instanceMap;
	unsigned int m_instanceCounter;

	HMMInstanceID m_iidCurrent;
	HMMInstanceID m_iidBkgd;

	// The Port grew out of the VideoTail - it represents a receptacle with a spot for each video stream
	// required ("Center", or "left"/"right"), and the optional spots in the audio mixer for sound.
	Port m_port;

	std::string getUri(const std::string& filename);

	// swap the iid in for current stim. dispose of current
	void swap(HMMInstanceID iid);

	// get next available stim instance id
	HMMInstanceID getNextInstanceID();

	// dispose of this stim or rewind as needed
	void dispose(HMMInstanceID iid);
	void removeStimInstance(HMMInstanceID iid);		// TODO: will throw, not trying to be pretty

	// initiates process to swap stim iid (assumed prerolled) for another by blocking it, then swapping in 'iid'.
	void initiateSwap(HMMInstanceID iid, Stim *pstimCurrent);

public:
	HMM(const HMMConfiguration& config, StimFactory& factory);
	virtual ~HMM();

	// Thread func that runs the GMainLoop and exits.
	static gpointer threadFunc(gpointer user_data);

	// const stim positions
	static const HMMStimPosition STIMPOS_AUDIO;
	static const HMMStimPosition STIMPOS_CONTROL;
	static const HMMStimPosition STIMPOS_LEFT;
	static const HMMStimPosition STIMPOS_CENTER;
	static const HMMStimPosition STIMPOS_RIGHT;

	// add a (single) file as a source
	HMMStimID addStim(const Habit::StimulusSettings& settings);
	Stim *getStimInstance(HMMInstanceID id);
	HMMInstanceID preroll(HMMStimID id);
	HMMInstanceID play(const HMMStimID& id); // does a preroll, but does the swap once its ready
	HMMInstanceID play(const HMMInstanceID& id);
	void dump(const char *c);

	// turn camera on/off
	void cam(bool bOn);

	// Port access
	Port &port() { return m_port; }

	// get stuff
	GstElement *pipeline() { return m_pipeline; }

	// Create Source* with the given file as input. Creates uridecodebin element, connects pad-added signal and no-more-pads signal
	// to callbacks. Element is left in READY state. The userdata passed is sent to the callbacks.
	// For these sources, the prerolling scheme is this: The counter is created with count=1. It is incremented
	// for each pad created (only pads we use - those we do not use are ignored and we do not increment counter).
	// As each pad is created,  a blocking probe is set, with the same counter passed. The no-more-pads callback
	// also decrements the counter (this matches the initial setting of 1). The blocking probe callback decrements
	// the counter. When the counter reaches 0 and fires, it initiates a flushing seek. The pad is already blocked,
	// so data will be flushed and flow again (this time with a SEGMENT event at the head). The parent counter,
	// which tallied the Sources being prerolled, is decremented as well.
	Source *makeSourceFromFile(const std::string& filename, HMMSourceType stype, void *userdata=NULL, bool loop = false, unsigned int volume=0);
	Source *makeSourceFromColor(unsigned long aarrggbb);

	// bus callback - bus messages here
	static gboolean busCallback(GstBus *, GstMessage *msg, gpointer pdata);
	static void padAddedCallback(GstElement * element, GstPad * pad, FileSourcePrerollCounter *pcounter);
	static void noMorePadsCallback(GstElement * element, FileSourcePrerollCounter *pcounter);
	static GstPadProbeReturn padProbeBlockCallback(GstPad * pad, GstPadProbeInfo * info, gpointer user_data);
	static GstPadProbeReturn eventProbeCallback(GstPad * pad, GstPadProbeInfo * info, gpointer p);
	static GstPadProbeReturn padProbeIdleCallback(GstPad *, GstPadProbeInfo *, gpointer user_data);

};

} // end namespace hmm

#endif /* HMM_H_ */
