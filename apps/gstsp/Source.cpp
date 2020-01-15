/*
 * HMMSource.cpp
 *
 *  Created on: Oct 17, 2019
 *      Author: dan
 */

#include "Source.h"
#include <sstream>
using namespace hmm;

Source::Source(HMMSourceType t, Stim *parent)
: m_sourceType(t)
, m_parent(parent)
{};

//void Source::addStream(HMMStreamType t, Stream *pstream)
//{
//	m_streamMap.insert(std::make_pair(t, pstream));
//}

Stream *Source::getStream(HMMStreamType t)
{
	Stream *pstream = NULL;
	if (m_streamMap.count(t) == 1)
		pstream = m_streamMap[t];
	return pstream;
}

void Source::addStream(HMMStreamType streamType, GstPad *srcpad)
{
	m_streamMap[streamType] = new Stream(srcpad, this);
}


ColorSource::ColorSource(HMMSourceType t, Stim *parent, unsigned int argb)
: Source(t, parent)
, m_argb(argb)
{
	std::ostringstream oss;
	GError *gerror = NULL;
	oss << "videotestsrc pattern=solid-color foreground-color=" << m_argb;
	m_ele = gst_element_factory_make("videotestsrc", NULL);
	if (m_ele == NULL)
	{
		throw std::runtime_error("Cannot create color source");
	}
	g_object_set (m_ele, "pattern", 17, "foreground-color", m_argb, NULL);
	GstPad *srcpad = gst_element_get_static_pad(m_ele, "src");
	addStream(HMMStreamType::VIDEO, srcpad);
}

GstElement *ColorSource::bin()
{
	return m_ele;
}

void ColorSource::preroll()
{
	g_print("prerolling color %x\n", m_argb);
}

void ColorSource::stop()
{

}

void ColorSource::rewind()
{

}

FileSource::FileSource(HMMSourceType t, Stim *parent, const std::string& filename, bool bloop, unsigned int volume)
: Source(t, parent)
, m_filename(filename)
, m_bloop(bloop)
, m_volume(volume)
{
	std::string uri("file://");
	uri.append(m_filename);
	m_ele = gst_element_factory_make("uridecodebin", NULL);
	if (!m_ele)
		throw std::runtime_error("gst_element_factory_make(uridecodebin) returned NULL");
	g_object_set (m_ele, "uri", uri.c_str(), NULL);
}

GstElement *FileSource::bin()
{
	return m_ele;
}

void FileSource::preroll()
{
	g_print("FileSource prerolling %s\n", m_filename.c_str());
}

void FileSource::stop()
{

}

void FileSource::rewind()
{

}
