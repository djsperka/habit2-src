/*
 * HabitStimFactory.cpp
 *
 *  Created on: Nov 21, 2019
 *      Author: dan
 */

#include "HabitStimFactory.h"
#include "HMM.h"
#include "stimulussettings.h"
#include <sstream>
#include <QtDebug>
#include <QString>

using namespace hmm;

HabitStimFactory::HabitStimFactory(const Habit::StimulusDisplayInfo& sdi, const QDir& rootDir)
: m_sdi(sdi)
, m_dir(rootDir)
{
	// create stimulus settings for the background.
	Habit::StimulusSettings background("background", sdi.getBackGroundColor());
	m_bkgdID = addStimulusSettings(background);
	g_print("HabitStimFactory::HabitStimFactory begin - sdi.layout %s\n", m_sdi.getStimulusLayoutType().name().toStdString().c_str());
	//qDebug() << background;
}

HabitStimFactory::~HabitStimFactory()
{
}

hmm::HMMStimID HabitStimFactory::addStimulusSettings(const Habit::StimulusSettings& settings)
{
	hmm::HMMStimID id = (hmm::HMMStimID)((unsigned long)m_ssMap.size());
	g_print("HabitStimFactory::addStimulusSettings - Add ss for id %d\n", (int)id);
	m_ssMap[id] = settings;
	g_print("HabitStimFactory::addStimulusSettings - stim name %s\n", m_ssMap[id].getName().toStdString().c_str());
	return id;
}

hmm::Stim* HabitStimFactory::background(hmm::HMM& mm)
{
	return operator()(m_bkgdID, mm);
}

hmm::Stim* HabitStimFactory::operator()(hmm::HMMStimID id, hmm::HMM& mm)
{
	hmm::Stim *pstim = NULL;
	g_print("HabitStimFactory::operator(%d)\n", (int)id);
	if (m_ssMap.count(id) == 0)
		throw std::runtime_error("StimFactory(id) not found");
	else
		pstim = makeHabitStim(m_ssMap[id], mm.pipeline());

	return pstim;
}

//hmm::Source *HabitStimFactory::makeSource(GstElement *pipeline, const Habit::StimulusInfo& info, void *userdata, hmm::HMMSourceType stype)
//{
//	hmm::Source *psrc=NULL;
//	if (info.isColor() || info.isBackground())
//	{
//		psrc = makeSourceFromColor(pipeline, info.getColor().rgba());
//	}
//	else
//	{
//		g_print("HabitStimFactory::makeSource(%s)\n", info.getAbsoluteFileName(m_dir).toStdString().c_str());
//		psrc = makeSourceFromFile(pipeline, info.getAbsoluteFileName(m_dir).toStdString(), stype, userdata, info.isLoopPlayBack(), info.getVolume());
//	}
//	return psrc;
//}
//
//hmm::Source *HabitStimFactory::makeSourceFromColor(GstElement *pipeline, unsigned long aarrggbb)
//{
//	std::ostringstream oss;
//	GError *gerror = NULL;
//	GstElement *src;
//	oss << "videotestsrc pattern=solid-color foreground-color=" << aarrggbb;
//	src = gst_parse_bin_from_description(oss.str().c_str(), true, &gerror);
//	if (src == NULL || gerror != NULL)
//		throw std::runtime_error("Cannot create color source");
//	gst_bin_add(GST_BIN(pipeline), src);
//	return new Source(hmm::HMMSourceType::VIDEO_ONLY, src, false, 0);
//}
//
//
//hmm::Source *HabitStimFactory::makeSourceFromFile(GstElement *pipeline, const std::string& filename, HMMSourceType stype, void *userdata, bool loop, unsigned int volume)
//{
//	std::string uri("file://");
//	uri.append(filename);
//	g_print("HMM::makeSourceFromFile(%s)\n", uri.c_str());
//	GstElement *ele = gst_element_factory_make("uridecodebin", NULL);
//	if (!ele)
//		g_print("NULL ele!\n");
//	g_object_set (ele, "uri", uri.c_str(), NULL);
//
//	// add ele to pipeline. pipeline takes ownership, will unref when ele is removed.
//	if (!gst_bin_add(GST_BIN(pipeline), ele))
//		g_print("ERROR- cannot add uridecodebin\n");
//
//	// Source does not ref the ele, assumes that its in a pipeline and ref'd there.
//	// TODO: not sure I like how ownership seems ill-defined.
//	Source *psrc = new Source(stype, ele, loop, volume);
//
//	g_signal_connect (psrc->bin(), "pad-added", G_CALLBACK(HMM::padAddedCallback), userdata);
//	g_signal_connect (psrc->bin(), "no-more-pads", G_CALLBACK(HMM::noMorePadsCallback), userdata);
//
//	return psrc;
//}

hmm::Stim *HabitStimFactory::makeHabitStim(const Habit::StimulusSettings& ss, GstElement *pipeline)
{
	Habit::StimulusInfo info;
	hmm::Stim *pstim=new hmm::Stim();
	pstim->setStimState(hmm::HMMStimState::NONE);

	// Now a bit of Habit messiness. The StimulusSettings object as originally written had
	// slots for 4 stimuli, and which ones were used depended on the StimulusDisplay settings for the experiment -
	// single screen, dual screen, etc.
	// In the HMM, the Port configuration is equivalent to the StimulusDisplay settings, but with
	// one complication: The "StimPosition" used here is similar to the "HPlayerPositionType" (left, center, right, iss)
	// used in habit. The complication is that the "StimPosition" is arbitrary here, whereas it had definite values
	// assigned in Habit. I've accounted for this by making the integer values for constants HMMSTimPosition::LEFT etc
	// equivalent to their HPlayerPositionType counterparts "number()" value.

	if (m_sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		// center
		addSourceToStim(pstim, HMM::STIMPOS_CENTER,
				(m_sdi.getUseISS() ? HMMSourceType::VIDEO_ONLY : HMMSourceType::AUDIO_VIDEO),
				pipeline, ss.getCenterStimulusInfo());

		// iss?
		if (m_sdi.getUseISS())
		{
			addSourceToStim(pstim, HMM::STIMPOS_AUDIO, HMMSourceType::AUDIO_ONLY,
					pipeline, ss.getIndependentSoundInfo());
		}
	}
	else
		throw std::runtime_error("HabitStimFactory::makeHabitStim() not implemented for l/r and triple layout");

//	else if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
//	{
//		// left
//		pstimCounter->increment();
//		pstim->addSource(HMM::STIMPOS_LEFT, makeSource(ss.getLeftStimulusInfo(), mm,
//				new SourcePrerollCounter(psrc, mm.pipeline(), 1, pstimCounter),
//				(sdi.getUseISS() ? HMMSourceType::VIDEO_ONLY : HMMSourceType::AUDIO_VIDEO)));
//
//		// right
//		pstimCounter->increment();
//		pstim->addSource(HMM::STIMPOS_RIGHT, makeSource(ss.getRightStimulusInfo(), mm,
//				new SourcePrerollCounter(psrc, mm.pipeline(), 1, pstimCounter),
//				(sdi.getUseISS() ? HMMSourceType::VIDEO_ONLY : HMMSourceType::AUDIO_VIDEO)));
//
//		// iss?
//		if (sdi.getUseISS())
//		{
//			pstimCounter->increment();
//			pstim->addSource(HMM::STIMPOS_AUDIO, makeSource(ss.getIndependentSoundInfo(), mm,
//					new SourcePrerollCounter(psrc, mm.pipeline(), 1, pstimCounter),
//					HMMSourceType::AUDIO_ONLY));
//		}
//	}
//	else if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutTriple)
//	{
//		// left
//		pstimCounter->increment();
//		pstim->addSource(HMM::STIMPOS_LEFT, makeSource(ss.getLeftStimulusInfo(), mm,
//				new SourcePrerollCounter(psrc, mm.pipeline(), 1, pstimCounter),
//				(sdi.getUseISS() ? HMMSourceType::VIDEO_ONLY : HMMSourceType::AUDIO_VIDEO)));
//
//		// right
//		pstimCounter->increment();
//		pstim->addSource(HMM::STIMPOS_RIGHT, makeSource(ss.getRightStimulusInfo(), mm,
//				new SourcePrerollCounter(psrc, mm.pipeline(), 1, pstimCounter),
//				(sdi.getUseISS() ? HMMSourceType::VIDEO_ONLY : HMMSourceType::AUDIO_VIDEO)));
//
//		// center
//		pstimCounter->increment();
//		pstim->addSource(HMM::STIMPOS_CENTER, makeSource(ss.getCenterStimulusInfo(), mm,
//				new SourcePrerollCounter(psrc, mm.pipeline(), 1, pstimCounter),
//				(sdi.getUseISS() ? HMMSourceType::VIDEO_ONLY : HMMSourceType::AUDIO_VIDEO)));
//
//		// iss?
//		if (sdi.getUseISS())
//		{
//			pstimCounter->increment();
//			pstim->addSource(HMM::STIMPOS_AUDIO, makeSource(ss.getIndependentSoundInfo(), mm,
//					new SourcePrerollCounter(psrc, mm.pipeline(), 1, pstimCounter),
//					HMMSourceType::AUDIO_ONLY));
//		}
//	}

	return pstim;
}

void HabitStimFactory::addSourceToStim(Stim *pstim, hmm::HMMStimPosition pos, hmm::HMMSourceType stype, GstElement *pipeline, const Habit::StimulusInfo& info)
{
	if (info.isColor() || info.isBackground())
	{
		pstim->addSource(pos, stype, pipeline, info.getColor().rgba());
	}
	else
	{
		//g_print("HabitStimFactory::makeSource(%s)\n", info.getAbsoluteFileName(m_dir).toStdString().c_str());
		pstim->addSource(pos, stype, pipeline, info.getAbsoluteFileName(m_dir).toStdString());
	}
}

