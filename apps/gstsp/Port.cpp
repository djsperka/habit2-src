/*
 * HMMPort.cpp
 *
 *  Created on: Oct 16, 2019
 *      Author: dan
 */

#include "Port.h"
#include "Stim.h"
#include <exception>
#include <sstream>
#include <QtDebug>

using namespace hmm;

Port::Port()
{
}

Port::~Port()
{
	// need to unref all this stuff....
	for (std::pair<HMMStimPosition, GstElement *> p : m_mapPosVideo)
	{
		gst_object_unref(p.second);
	}
	for (std::pair<HMMStimPosition, GstElement *> p : m_mapPosAudio)
	{
		gst_object_unref(p.second);
	}
	for (std::pair<HMMStimPosition, GstElement *> p : m_mapPosVideoSink)
	{
		gst_object_unref(p.second);
	}
	for (std::pair<HMMStimPosition, GstElement *> p : m_mapPosAudioSink)
	{
		gst_object_unref(p.second);
	}
}

void Port::addVideoEle(HMMStimPosition pos, GstElement *ele)
{
	if (m_mapPosVideo.count(pos) > 0)
		throw std::runtime_error("port already has ele at this stim position");
	m_mapPosVideo[pos] = ele;
}

void Port::addAudioEle(HMMStimPosition pos, GstElement *ele)
{
	if (m_mapPosAudio.count(pos) > 0)
		throw std::runtime_error("port already has ele at this stim position");
	m_mapPosAudio[pos] = ele;
}

void Port::addVideoSink(HMMStimPosition pos, GstElement *sink)
{
	if (m_mapPosVideoSink.count(pos) > 0)
		throw std::runtime_error("port already has video sink at this stim position");
	m_mapPosVideoSink[pos] = sink;
}

void Port::addAudioSink(HMMStimPosition pos, GstElement *sink)
{
	if (m_mapPosAudioSink.count(pos) > 0)
		throw std::runtime_error("port already has audio sink at this stim position");
	m_mapPosAudioSink[pos] = sink;
}

void Port::connect(Stim& stim)
{
	// check that the port has nothing connected...
	g_print("Port::connect() - start\n");
	std::ostringstream oss;
	for (auto it: m_mapPosVideo)
	{
		GstPad *pad = gst_element_get_static_pad(it.second, "sink");
		g_print("conect() - pad? %s\n", (pad ? "YES" : "NO"));
		if (gst_pad_is_linked(pad))
		{
			oss << "port pad \"" << it.first << " still connected. ";
			throw std::runtime_error(oss.str().c_str());
		}
		g_print("unref\n");
		gst_object_unref(pad);
		g_print("unref done\n");
	}


	/*
	 *
	 *
	 * 12345r7890qetyuilzxccvbnm,./
	 *
	 * - Bronte 4/8/200000[3oliuuhhfxxzzxvcxxncxxxxxxxxcccc 000000000000xxx
	 * 0009000000900099853900000qwerzxevx000000000nnbvcssxgh000000-/00pp;ln0000------[[-bxzqefbbn nfhn555rxd5qwesszzzjccd3e;kjgfsqweytrtrerggfdxawaww3q1234567890qwertyuyipdmmkkh./dgndgnnf23570mbzx,mfguujjyxxxxxxxxxxxxxxxppppppppppppu6665555h7777777777777777777j99999gwvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv,gtemhcgytgrtytu6aoooooooooopm,nOtf8vsou76tuiuj76513vbmvhtwwe555rewwww245xcfdsxzw332u7h123457--=zqert31hj,.mbcxzaaw321zaxcbre21vnjfsvvbn71av000000000-00000000000999098h98777333323pppoutrjxmb21256uioop12370-fgx322`zzzxcvm,uu7123456778
	 * "m    tb111199rstaxccccccccccccccccccccccxccccccccccccccccccccccccccccccccccccccccccccccccccnfnr888888888888ddddwfgyujnbhyuhbhyhbvgytgbv gt6yhb bgyujhgyuijhyuikjuikjjhjhhhhvjg yi8mm
	 *
	 * 12346567890qwewettyuiopzxxcvvvbnm12223rrrrrrrr1111111111111234567890qwertyyyuuiioppzxcvbmnm,,1
	 * hbbhkll,,mmmmmm  mm,mmm,mmmmmmmm8ykkr3k,lo3
	 * 323455cccccxtuffgvfgtrewqxnm312345678900000000qqwerrrrtyuuiopzxgavu3bbcmzaqe5e168900
	 *
	 *
	 *
	 * ldq2lvuffjcfjgcxxzi.,653w34 bxhnnnnnn333333333333333333333333222222222331211bbbbbbbvbmmmmmmmmm55r5yoo6rhvew4uluhffddee3tyhhg vlui1233333333w3333o[hsyiobyy44er444444444444u8888bgz33337ywrhbffdsea3qw;ddsdsebew33jkhfsw2333211122ryyyhjjjjjj[yteswwwwwseedddfhhhsdqvf,,,cx
	 *
	 *
	 */

	// OK now connect all video streams in 'stim' to the port.
	Stim::HMMStimPosSourceMap::iterator it;
	g_print("start connecting... %d sources\n", stim.sourceMap().size());
	for (it = stim.sourceMap().begin(); it!= stim.sourceMap().end(); it++)
	{
		g_print("Port::connect(): SourcePosition %s SourceType %d\n", it->first.c_str(), (int)(it->second->sourceType()));

		// does this source have a video stream?
		Stream *pstream = it->second->getStream(HMMStreamType::VIDEO);
		if (pstream)
		{
			g_print("Port::connect(): Found a video stream for this source\n");
			if (m_mapPosVideo.count(it->first) > 0)
			{
				GstPad *pad = gst_element_get_static_pad(m_mapPosVideo[it->first], "sink");

				// the pendign stream should have a connected fake sink.
				if (gst_pad_is_linked(pstream->srcpad()))
				{
					g_print("Port::connect(): This stream is linked, unlinking...\n");
					GstPad *l = gst_pad_get_peer(pstream->srcpad());
					GstElement *sink = GST_ELEMENT_CAST(gst_pad_get_parent(l));
					gst_element_set_state(sink, GST_STATE_NULL);
					gst_bin_remove(GST_BIN(stim.pipeline()), sink);
					gst_object_unref(l);
					gst_object_unref(sink);
				}

				/*
				 * BXVVNMO12344567890QERTYUIOPZXCCCVBNNNNM, - BRONTE
				 */

				GstPadLinkReturn r = gst_pad_link(pstream->srcpad(), pad);
				if (r != GST_PAD_LINK_OK)
				{
					g_print("Port::connect():pad link error %d\n", (int)r);
					throw std::runtime_error("Cannot link pads.");
				}
				// set offset on the downstream pad, but not when initializing the pipeline.
				GstClock *clock = gst_element_get_clock(m_mapPosVideo[it->first]);
				if (clock)
				{
					GstClockTime abs = gst_clock_get_time(clock);
					GstClockTime base = gst_element_get_base_time(m_mapPosVideo[it->first]);
					g_print("setting pad offset to %u ms", GST_TIME_AS_MSECONDS(abs-base));
					gst_pad_set_offset(pad, abs-base);
					gst_object_unref(clock);
				}
				else
				{
					g_print("Port::connect(): linked, not setting offset on pad.\n");
				}
				gst_object_unref(pad);

				// finally, remove blocking probe
				if (pstream->getProbeID())
				{
					g_print("Port::connect(): removed blocking probe\n");
					gst_pad_remove_probe(pstream->srcpad(), pstream->getProbeID());
					pstream->setProbeID(0);
				}
			}
			else
			{
				oss << "Stim pos " << it->first << " port does not have video ele at this pos ";
				throw std::runtime_error(oss.str().c_str());
			}
		}
		else
		{
			g_print("Port::connect(): NO VIDEO STREAM FOUND for this source\n");
		}



		// does this source have a audio stream?
		pstream = it->second->getStream(HMMStreamType::AUDIO);
		if (pstream)
		{
			g_print("Port::connect(): Found an audio stream for this source - WARNING\n");

//			// Just unblock it
//
//			// set offset on the downstream pad, but not when initializing the pipeline.
//			GstClock *clock = gst_element_get_clock(m_mapPosVideo[it->first]);
//			if (clock)
//			{
//				GstClockTime abs = gst_clock_get_time(clock);
//				GstClockTime base = gst_element_get_base_time(m_mapPosVideo[it->first]);
//				g_print("setting AUDIO pad offset to %u ms", GST_TIME_AS_MSECONDS(abs-base));
//				GstPad *pad = gst_pad_get_peer(pstream->srcpad());
//				gst_pad_set_offset(pad, abs-base);
//				gst_object_unref(clock);
//				gst_object_unref(pad);
//			}
//			else
//			{
//				g_print("Port::connect(): linked, not setting offset on pad.\n");
//			}
//
			gst_pad_remove_probe(pstream->srcpad(), pstream->getProbeID());
			pstream->setProbeID(0);
		}
	}
	g_print("Port::connect() - done\n");
}

void Port::disconnect()
{
	// Any pads connected will be unceremoniously disconnected.
	// Any request pads from the mixer will be returned

//	for (std::list<GstPad *>::iterator it = m_listRequestPads.begin(); it != m_listRequestPads.end(); it++)
//	{
//		throw std::runtime_error("HMMPort::disconnect () not implemented for audio");
//	}
	g_print("Port::disconnect() not implemented for audio\n");

	// Iterate over the "ele" map - these are the first elements in the tail for a particular
	// position. The sink pad on those elements are connected to the stim - unceremoniously
	// unlink them here. This will leave you with a stim that has no tail connected -
	// it should be blocked on the pad opposite this one.
	for (HMMPortPosEleMap::iterator it = m_mapPosVideo.begin(); it != m_mapPosVideo.end(); it++)
	{
		GstPad *pad = gst_element_get_static_pad(m_mapPosVideo[it->first], "sink");
		if (gst_pad_is_linked(pad))
		{
			gst_pad_unlink(gst_pad_get_peer(pad), pad);
		}
		gst_object_unref(pad);
	}
}

std::vector<HMMStimPosition> Port::getVideoPositions() const
{
	std::vector<HMMStimPosition> vec;
	for (HMMPortPosEleMap::const_iterator it = m_mapPosVideo.begin(); it!= m_mapPosVideo.end(); it++)
		vec.push_back(it->first);
	return vec;
}

std::vector<HMMStimPosition> Port::getAudioPositions() const
{
	std::vector<HMMStimPosition> vec;
	for (HMMPortPosEleMap::const_iterator it = m_mapPosAudio.begin(); it!= m_mapPosAudio.end(); it++)
		vec.push_back(it->first);
	return vec;
}

void Port::setWidget(HMMStimPosition pos, GValue& value)
{
	// find sink for 'pos'
	if (m_mapPosVideoSink.count(pos) == 1)
	{
		g_object_set_property(G_OBJECT(m_mapPosVideoSink[pos]), "widget", &value);
	}
	else
		throw std::runtime_error("Cannot set port at this pos");
}

