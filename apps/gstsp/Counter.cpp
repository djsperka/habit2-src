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
	g_print("Counter::decrement() dec to %d\n", m_counter);
	return false;
}

void ColorSourcePrerollCounter::operator()(void)
{
	g_print("ColorSourcePrerollCounter triggered - color stream is prerolled.\n");
}

void ImageSourcePrerollCounter::operator()(void)
{
	g_print("ImageSourcePrerollCounter triggered - image stream is prerolled.\n");
}

void FileSourcePrerollCounter::operator()(void)
{
	if (m_bIsImage)
	{
		g_print("FileSourcePrerollCounter triggered - image stream is prerolled.\n");
	}
	else
	{
		if (!this->fileSource()->isSeeking())
		{
			if (this->fileSource()->isLoop())
			{
				// this is when we are first filling the pipeline, but a segment seek has not yet been issued.
				// Set the 'seeking' flag, post a message on the bus.
				// This object is still attached to the probe, so we should reset it to the number of streams.
				this->fileSource()->setSeeking(true);
				this->setCounter(this->fileSource()->streamMap().size());
				GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline()));
				GstStructure *structure = gst_structure_new("seek", "psrc", G_TYPE_POINTER, this->fileSource(),  NULL);
				gst_bus_post (bus, gst_message_new_application(GST_OBJECT_CAST(this->fileSource()->bin()), structure));
				gst_object_unref(bus);
				g_print("FileSourcePrerollCounter (loop) triggered - segment seek issued, reset counter to %d\n", this->fileSource()->streamMap().size());
			}
			else
			{
				this->fileSource()->parentStim()->setStimState(HMMStimState::PREROLLED);
				g_print("FileSourcePrerollCounter (not loop) triggered - source is prerolled.\n");
			}
		}
		else
		{
			g_print("FileSourcePrerollCounter triggered - (was seeking) - all streams are prerolled.\n");
		}
	}
}

void StimPrerollCounter::operator()(void)
{
	g_print("StimPrerollCounter triggered\n");
	stim()->setStimState(HMMStimState::PREROLLED);
}

void NoopCounter::operator()(void)
{
	g_print("HMMNoopCounter triggered\n");
}

void StimSwapCounter::operator()(void)
{
	g_print("StimSwapCounter triggered\n");
	// post bus message
	GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(this->hmm()->pipeline()));
	GstStructure *structure = gst_structure_new("swap", "iid", G_TYPE_ULONG, this->iid(), NULL);
	gst_bus_post (bus, gst_message_new_application(GST_OBJECT_CAST(hmm()->pipeline()), structure));
	gst_object_unref(bus);
}

void StimPlayCounter::operator()(void)
{
	Stim *pstim = hmm()->getStimInstance(iid());
	pstim->setStimState(HMMStimState::PREROLLED);
	g_print("StimPlayCounter triggered - pending stim (iid=%lu) is prerolled and ready\n", iid());
	hmm()->play(iid());
}

