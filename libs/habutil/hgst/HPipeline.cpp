/*
 * HPipeline.cpp
 *
 *  Created on: Nov 1, 2017
 *      Author: dan
 */

#include "HPipeline.h"
#include "HGMMException.h"

HPipeline::HPipeline(int id, const Habit::StimulusSettings& ss, const Habit::StimulusDisplayInfo& info, QObject *parent)
: QObject(parent)
, m_id(id)
, m_ss(ss)
, m_sdinfo(info)
{
}

void HPipeline::reconfigure(const Habit::StimulusDisplayInfo& info)
{
	//cleanup();
	m_sdinfo = info;
}


void HPipeline::detachWidgetsFromSinks()
{
	//no-op
}

void HPipeline::attachWidgetsToSinks(HVideoWidget *, HVideoWidget *)
{
	// no-op
}

void HPipeline::dump()
{
	qDebug() << "HPipeline::dump not implemented!";
}



QString HPipeline::makeElementName(const char *factoryName, const HPlayerPositionType& ppt, int number, const char *prefix)
{
	QString result;
	if (!prefix)
	{
		QString format("%1-%2-%3");
		result = format.arg(QString(factoryName)).arg(ppt.name()).arg(number);
	}
	else
	{
		QString format("%1-%2-%3-%4");
		result = format.arg(QString(prefix)).arg(QString(factoryName)).arg(ppt.name()).arg(number);
	}
	return result;
}

GstElement *HPipeline::makeElement(const char *factoryName, const HPlayerPositionType& ppt, int number, const char *prefix)
{
	return gst_element_factory_make(factoryName, makeElementName(factoryName, ppt, number, prefix).toStdString().c_str());
}

bool HPipeline::parseElementName(const QString& elementName, QString& factoryName, const HPlayerPositionType*& pppt, int& id, QString& prefix)
{
	bool b = false;
	int iPrefix=0;
	QStringList l = elementName.toLower().split('-');
	if (l.size() == 4) iPrefix = 1;
	else if (l.size() != 3) return false;

	factoryName = l[0 + iPrefix];

	if (l[1 + iPrefix] == QString("left"))
		pppt = &HPlayerPositionType::Left;
	else if (l[1 + iPrefix] == QString("right"))
		pppt = &HPlayerPositionType::Right;
	else if (l[1 + iPrefix] == QString("center"))
		pppt = &HPlayerPositionType::Center;
	else if (l[1 + iPrefix] == QString("sound"))
		pppt = &HPlayerPositionType::Sound;
	else if (l[1 + iPrefix] == QString("control"))
		pppt = &HPlayerPositionType::Control;
	else
		return false;

	id = l[2 + iPrefix].toInt(&b);
	return b;
}

void HPipeline::parseCaps(GstCaps* caps, bool& isVideo, bool& isImage, int& width, int& height, bool& isAudio)
{
	GstStructure *new_pad_struct = NULL;
	const gchar *new_pad_type = NULL;
	gchar *s_new_pad_caps = NULL;
	const GValue *v = NULL;
	new_pad_struct = gst_caps_get_structure(caps, 0);
	new_pad_type = gst_structure_get_name(new_pad_struct);
	s_new_pad_caps = gst_caps_to_string(caps);
	qDebug() << "parseCaps: " << s_new_pad_caps;

	isAudio = g_str_has_prefix(s_new_pad_caps, "audio/x-raw");
	if (g_str_has_prefix(s_new_pad_caps, "video/x-raw"))
	{
		// get width/height
		v = gst_structure_get_value(new_pad_struct, "width");
		if (v && G_VALUE_HOLDS_INT(v))
			width = g_value_get_int(v);
		else
			qCritical() << "Width is not an int!?!";

		v = gst_structure_get_value(new_pad_struct, "height");
		if (v && G_VALUE_HOLDS_INT(v))
			height = g_value_get_int(v);
		else
			qCritical() << "Height is not an int!?!";

		// check framerate
		v = gst_structure_get_value(new_pad_struct, "framerate");
		if (v && GST_VALUE_HOLDS_FRACTION(v))
		{
			gint num = gst_value_get_fraction_numerator(v);
			if (num == 0)
			{
				isImage = true;
				isVideo = false;
			}
			else if (num > 0)
			{
				isImage = false;
				isVideo = true;
			}
			else qCritical() << "negative numerator?";
		}
		else qCritical() << "framerate not in caps or unexpected value type";
	}
	qDebug() << "parseCaps: Video? " << isVideo << " Image? " << isImage << " Audio? :" << isAudio << " Resolution: " << width << "x" << height;
}


void HPipeline::write(std::ostream& os) const
{
    os << "Pipeline Key: " << id() << std::endl;
}

std::ostream& operator<<(std::ostream& os, const HPipeline& p)
{
    p.write(os);
    return os;
}
