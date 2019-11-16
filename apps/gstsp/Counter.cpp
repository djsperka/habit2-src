/*
 * HMMCounter.cpp
 *
 *  Created on: Oct 17, 2019
 *      Author: dan
 */

#include "Counter.h"
#include "HMM.h"
using namespace hmm;

void Counter::increment()
{
	g_atomic_int_inc(&m_counter);
//	g_print("inc to %d\n", m_counter);
}

bool Counter::decrement()
{
	if (g_atomic_int_dec_and_test(&m_counter))
	{
//		g_print("dec to %d\n", m_counter);
		this->operator()();

		// if a sub counter exists, decrement that one now
		if (m_psubCounter)
			m_psubCounter->decrement();
		return true;
	}
//	g_print("dec to %d\n", m_counter);
	return false;
}


void SourcePrerollCounter::operator()(void)
{
	g_print("SourcePrerollCounter triggered\n");
	GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline()));
	GstStructure *structure = gst_structure_new("seek", "psrc", G_TYPE_POINTER, this->source(),  NULL);
	gst_bus_post (bus, gst_message_new_application(GST_OBJECT_CAST(this->source()->bin()), structure));
	gst_object_unref(bus);
}

void StimPrerollCounter::operator()(void)
{
	stim()->setStimState(HMMStimState::PREROLLED);
}

void NoopCounter::operator()(void)
{
	//g_print("HMMNoopCounter triggered\n");
}

void PlayStimCounter::operator()(void)
{
	// post bus message
	GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(this->hmm()->pipeline()));
	GstStructure *structure = gst_structure_new("play", "current", G_TYPE_POINTER, this->current(), "pending", G_TYPE_POINTER, this->pending(),  NULL);
	gst_bus_post (bus, gst_message_new_application(GST_OBJECT_CAST(hmm()->pipeline()), structure));
	gst_object_unref(bus);
}


//void StimSwapCounter::operator()(void)
//{
//	//g_print("StimSwapCounter triggered\n");
//
//	for (std::pair<const HMMStimPosition, Stim::SourceP> p: hmm()->getStim(current())->sourceMap())
//	{
//		if (p.first > 0)
//		{
//			Source* psrc = p.second;
//			Source* psrcPending = hmm()->getStim(pending())->getSource(p.first);
//			if (!psrcPending)
//				throw std::runtime_error("Pending source not found corresponding to current src");
//			// does current source have a video stream?
//			Stream *pvideoCurrent = psrc->getStream(HMMStreamType::VIDEO);
//			Stream *pvideoPending = psrcPending->getStream(HMMStreamType::VIDEO);
//
//			if (pvideoCurrent)
//			{
//				if (!pvideoPending)
//					throw std::runtime_error("Pending source missing video stream");
//
//				// unlink
//				HMMVideoTail* ptail = hmm()->getVideoTail(p.first);
//				if (!ptail)
//					throw std::runtime_error("cannot find video tail");
//				GstPad *psink = gst_element_get_static_pad(ptail->m_conv, "sink");
//				gst_pad_unlink(pvideoCurrent->srcpad(), psink);
//
//				// link
//				gst_pad_link(pvideoPending->srcpad(), psink);
//
//				// set offset on tail pad
//				GstClockTime abs = gst_clock_get_time(gst_element_get_clock(ptail->m_conv));
//				GstClockTime base = gst_element_get_base_time(ptail->m_conv);
//				gst_pad_set_offset(psink, abs-base);
//				gst_object_unref(psink);
//
//				g_print("Set offset on tail pad to %lums\n", GST_TIME_AS_MSECONDS(abs-base));
//				// remove probe on the pending video src pad
//				gst_pad_remove_probe(pvideoPending->srcpad(), pvideoPending->getProbeID());
//
////				// finally, do flushing seek on removed source
////				// TODO - the actual disposition of the source really controlled at the stim level
////
//				GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(this->hmm()->pipeline()));
//				GstStructure *structure = gst_structure_new("seek", "id", G_TYPE_ULONG, this->current(), "pos", G_TYPE_INT, p.first,  NULL);
//				gst_bus_post (bus, gst_message_new_application(GST_OBJECT_CAST(this->hmm()->pipeline()), structure));
//				gst_object_unref(bus);
//
//
//			}
//			else
//				throw std::runtime_error("Current source has video pos but no video stream");
//		}
//	}
//}
//



