/*
 * MM1.h
 *
 *  Created on: Sep 6, 2019
 *      Author: dan
 */

#ifndef MM1_H_
#define MM1_H_

#include <string>
#include <gst/gst.h>
#include <vector>

class MM1
{
	bool m_bVideo, m_bAudio;
	GstElement *m_pipeline;
	GstElement *m_bkgd;
	GstElement *m_conv, *m_scale, *m_vsink;	// pipeline video goes through in order conv-scale-vsink

	typedef struct
	{
		GstPad *srcpad;
		gulong probeid;
	} PadStruct;

	typedef struct
	{
		std::string filename;
		GstElement *filesrc, *dbin;
	} SourceStruct;

	std::vector<SourceStruct> m_sources;
	unsigned int m_uiCurrentSource;

public:
	MM1(bool bVideo, bool bAudio=false, const std::string& bkgd="videotestsrc");
	virtual ~MM1();


	// add a (single) file as a source
	unsigned int addSource(const std::string& filename);
	bool preroll(unsigned int id);

	// bus callback - bus messages here
	static gboolean busCallback(GstBus *, GstMessage *msg, gpointer pdata);

};

#endif /* MM1_H_ */
