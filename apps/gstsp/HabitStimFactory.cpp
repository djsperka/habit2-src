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
	m_ssMap[id] = settings;
	g_print("HabitStimFactory::addStimulusSettings() %d: %s\n", (int)id, settings.getName().toStdString().c_str());
	return id;
}

hmm::Stim* HabitStimFactory::background(hmm::HMM& mm)
{
	return operator()(m_bkgdID, mm, "BKGD");
}

hmm::Stim* HabitStimFactory::operator()(hmm::HMMStimID id, hmm::HMM& mm, const std::string& prefix)
{
	hmm::Stim *pstim = NULL;
	g_print("HabitStimFactory::operator(%d)\n", (int)id);
	if (m_ssMap.count(id) == 0)
		throw std::runtime_error("StimFactory(id) not found");
	else
		pstim = makeHabitStim(m_ssMap[id], mm.pipeline(), prefix);

	return pstim;
}

hmm::Stim *HabitStimFactory::makeHabitStim(const Habit::StimulusSettings& ss, GstElement *pipeline, const std::string& prefix)
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
		std::string posPrefix(prefix);
		posPrefix.append("-C");
		addSourceToStim(pstim, posPrefix, HMM::STIMPOS_CENTER,
				(m_sdi.getUseISS() ? HMMSourceType::VIDEO_ONLY : HMMSourceType::AUDIO_VIDEO),
				pipeline, ss.getCenterStimulusInfo());

		// iss?
		if (m_sdi.getUseISS())
		{
			std::string issPrefix(prefix);
			posPrefix.append("-A");
			addSourceToStim(pstim, issPrefix, HMM::STIMPOS_AUDIO, HMMSourceType::AUDIO_ONLY,
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

void HabitStimFactory::addSourceToStim(Stim *pstim, const std::string& prefix, hmm::HMMStimPosition pos, hmm::HMMSourceType stype, GstElement *pipeline, const Habit::StimulusInfo& info)
{
	GstElement *ele = NULL;
	if (info.isColor() || info.isBackground())
	{
		std::ostringstream oss;
		oss << prefix << "-videotestsrc";
		ele = gst_element_factory_make("videotestsrc", oss.str().c_str());
		if (ele == NULL)
		{
			throw std::runtime_error("Cannot create color source");
		}
		oss.clear();
		oss << prefix << "-fakesink";
		GstElement *sink = gst_element_factory_make("fakesink", oss.str().c_str());
		gst_bin_add_many(GST_BIN(pipeline), ele, sink, NULL);
		gst_element_link(ele, sink);
		//g_object_set (ele, "pattern", 17, "foreground-color", info.getColor().rgba(), NULL);

		ColorSource *src = new ColorSource(stype, ele);
		GstPad *srcpad = gst_element_get_static_pad(ele, "src");
		src->addStream(HMMStreamType::VIDEO, srcpad, sink);
		g_print("ColorSource %x done m_ele %p\n", info.getColor().rgba(), ele);
		pstim->addSource(pos, src);
	}
	else
	{
		std::string uri("file://");
		std::string filename(info.getAbsoluteFileName(m_dir).toStdString());
		uri.append(filename);
		std::ostringstream oss;
		oss << prefix << "-uridecodebin";
		ele = gst_element_factory_make("uridecodebin", oss.str().c_str());
		gst_bin_add(GST_BIN(pipeline), ele);
		g_print("FileSource %s done m_ele %p\n", filename.c_str(), ele);
		if (!ele)
			throw std::runtime_error("gst_element_factory_make(uridecodebin) returned NULL");
		g_object_set(ele, "uri", uri.c_str(), NULL);
		FileSource *f = new FileSource(stype, ele, info.isLoopPlayBack(), info.getVolume());
		pstim->addSource(pos, f);
	}
}

