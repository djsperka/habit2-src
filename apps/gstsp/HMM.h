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
#include <gst/gst.h>
#include "stimulussettings.h"
#include "Stream.h"
#include "Source.h"
#include "Stim.h"
#include "Counter.h"
#include "Port.h"


namespace hmm {

struct HMMVideoTail
{
	GstElement *m_conv, *m_scale, *m_vsink;
};


struct HMMConfiguration
{
	std::map<HMMStimPosition, std::string> video;
	std::map<HMMStimPosition, std::string> audio;
};

class HMM
{
	bool m_bVideo, m_bAudio;
	GstElement *m_pipeline;

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

	//typedef std::pair<bool, std::string> stim_info;	// first: true->isFile, false->is a bin description, e.g. "videotestsrc"
	//std::map<HMMStimID, stim_info> m_stimInfoMap;

	std::map<HMMStimID, Habit::StimulusSettings> m_ssMap;
	typedef std::unique_ptr<Stim> stim_ptr;
	std::map<HMMInstanceID, stim_ptr> m_instanceMap;

	HMMInstanceID m_iidCurrent;
	HMMInstanceID m_iidBkgd;

	// The Port grew out of the VideoTail - it represents a receptacle with a spot for each video stream
	// required ("Center", or "left"/"right"), and the optional spots in the audio mixer for sound.
	//
	//std::map<HMMStimPosition, HMMVideoTail> m_stimTailMap;
	Port m_port;

	std::string getUri(const std::string& filename);


	// substitute stim with 'id' for current stim, make it current (return previous stimid)
	//HMMStimID swap(HMMStimID id);

	// factory
	Stim *makeStim(const Habit::StimulusSettings& ss);
	//Stim* makeStimFromFile(HMMStimID id, const std::string& filename);
	//Stim* makeStimFromDesc(HMMStimID id, const std::string& description);
	Source *makeSourceFromFile(const std::string& filename, HMMSourceType stype, bool loop = false, unsigned int volume=0);


public:
	HMM(const HMMConfiguration& config);
	virtual ~HMM();

	// const stim positions
	static const HMMStimPosition STIMPOS_AUDIO;
	static const HMMStimPosition STIMPOS_CONTROL;
	static const HMMStimPosition STIMPOS_LEFT;
	static const HMMStimPosition STIMPOS_CENTER;
	static const HMMStimPosition STIMPOS_RIGHT;


	// add a (single) file as a source
	//HMMStimID addStimInfo(const std::string& filename_or_description, bool bIsFile=true);
	HMMStimID addStim(const Habit::StimulusSettings& settings);
	Stim *getStimInstance(HMMInstanceID id);
	HMMInstanceID preroll(HMMStimID id);
	void play(const HMMInstanceID& id);
	void dump(const char *c);

	// tail
	//void addVideoTail(HMMStimPosition pos, HMMVideoTail tail) { m_stimTailMap[pos] = tail; }
	//HMMVideoTail* getVideoTail(HMMStimPosition pos);

	// Port access
	Port &port() { return m_port; }

	// get stuff
	GstElement *pipeline() { return m_pipeline; }

	// bus callback - bus messages here
	static gboolean busCallback(GstBus *, GstMessage *msg, gpointer pdata);
	static void padAddedCallback(GstElement * element, GstPad * pad, SourcePrerollCounter *pcounter);
	static void noMorePadsCallback(GstElement * element, SourcePrerollCounter *pcounter);
	static GstPadProbeReturn padProbeBlockCallback(GstPad * pad, GstPadProbeInfo * info, gpointer user_data);
	static GstPadProbeReturn eventProbeCallback(GstPad * pad, GstPadProbeInfo * info, gpointer p);
	static GstPadProbeReturn padProbeIdleCallback(GstPad *, GstPadProbeInfo *, gpointer user_data);

};

} // end namespace hmm

#endif /* HMM_H_ */
