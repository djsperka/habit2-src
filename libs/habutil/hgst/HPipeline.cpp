/*
 * HPipeline.cpp
 *
 *  Created on: Nov 1, 2017
 *      Author: dan
 */

#include "HPipeline.h"
#include "HPipelineColor.h"
#include "HGMMException.h"

HPipeline::HPipeline(int id, QObject *parent)
: QObject(parent)
, m_id(id)
{
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

//HPipeline *HPipeline::createPipeline(int id, const Habit::StimulusSettings& stimulusSettings, const QDir& stimRoot, const HStimulusLayoutType& stimulusLayoutType, bool bSound, bool bISS)
//{
//	if (stimulusLayoutType == HStimulusLayoutType::HStimulusLayoutSingle)
//	{
//		return createPipelineSingle(id, stimulusSettings, stimRoot, bSound, bISS);
//	}
//	else if (stimulusLayoutType == HStimulusLayoutType::HStimulusLayoutLeftRight)
//	{
//		return NULL;
//		createPipelineLeftRight(id, stimulusSettings, stimRoot, bSound, bISS);
//	}
//	else
//	{
//		throw HGMMException(std::string("Unknown stimulus layout type: ") + C_STR(stimulusLayoutType.name()));
//	}
//}


//HPipeline *HPipeline::createPipelineSingle (int id, const Habit::StimulusSettings& stimulusSettings, const QDir& stimRoot, bool bSound, bool bISS)
//{
//	HPipeline *pipeline = NULL;
//
//	// is center stimulus a solid color or a file?
////	if (stimulusSettings.getCenterStimulusInfo().isColor() || stimulusSettings.getCenterStimulusInfo().isBackground())
////	{
////		return new HPipelineColor(id, stimulusSettings.getCenterStimulusInfo().getColor(), bISS);
////	}
//	return pipeline;
//}
//
//HPipeline *HPipeline::createPipelineLeftRight(int id, const Habit::StimulusSettings& stimulusSettings, const QDir& stimRoot, bool bSound, bool bISS)
//{
//	HPipeline *pipeline = NULL;
//	return pipeline;
//}

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
	else if (l[1 + iPrefix] == QString("control"))
		pppt = &HPlayerPositionType::Control;
	else
		return false;

	id = l[2 + iPrefix].toInt(&b);
	return b;
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
