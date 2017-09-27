#include "stimulusdisplayinfo.h"

#include <maindao.h>

// version string for input/output. See operator<<, operator>>
static const QString f_sVersion2("SDI2");

Habit::StimulusDisplayInfo::StimulusDisplayInfo() 
: id_(-1)
, pstyle_(&HPresentationStyle::HPresentationStyleUnknown)
, pdtype_(&HDisplayType::HDisplayTypeFullScreen)
, isOriginalAspectRatioMaintained_(true)
, backGroundColor_(0, 0, 0, 255)
, playouttype_(&HStimulusLayoutType::HStimulusLayoutSingle)
, useISS_(false)
{
}

Habit::StimulusDisplayInfo::StimulusDisplayInfo(const StimulusDisplayInfo& info)
{
	id_ = info.getId();
	pstyle_ = &info.getPresentationStyle();
	pdtype_ = &info.getDisplayType();
	isOriginalAspectRatioMaintained_ = info.isOriginalAspectRatioMaintained();
	backGroundColor_ = info.getBackGroundColor();
	playouttype_ = &info.getStimulusLayoutType();
	useISS_ = info.getUseISS();
}

Habit::StimulusDisplayInfo& Habit::StimulusDisplayInfo::operator=(const Habit::StimulusDisplayInfo& rhs)
{
	if (this != &rhs)
	{
		setId(rhs.getId());
		setPresentationStyle(rhs.getPresentationStyle());
		setDisplayType(rhs.getDisplayType());
		setMaintainOriginalAspectRatio(rhs.isOriginalAspectRatioMaintained());
		setBackGroundColor(rhs.getBackGroundColor());
		setStimulusLayoutType(rhs.getStimulusLayoutType());
		setUseISS(rhs.getUseISS());
	}
	return *this;
}

Habit::StimulusDisplayInfo Habit::StimulusDisplayInfo::clone() const
{
	Habit::StimulusDisplayInfo info(*this);
	info.setId(-1);
	return info;
}


Habit::StimulusDisplayInfo::~StimulusDisplayInfo()
{
}


QDataStream & Habit::operator<< (QDataStream& stream, Habit::StimulusDisplayInfo d)
{
//	stream.writeBytes(f_psVersion2, 4);
	stream << f_sVersion2 << d.getId() << d.getPresentationStyle().number() << d.getDisplayType().number() << d.isOriginalAspectRatioMaintained() << d.getBackGroundColor() << d.getStimulusLayoutType().number() << d.getUseISS();
	return stream;
}

QDataStream & Habit::operator>> (QDataStream& stream, Habit::StimulusDisplayInfo& d)
{
	int id, istyle, itype, ilayout;
	bool b;
	bool use;
	QColor color;
	QString sVersion;

	// Save position in stream in case this is an old
	qint64 pos = stream.device()->pos();
	stream >> sVersion;
	if (sVersion == f_sVersion2)
	{
		// now read
		stream >> id >> istyle >> itype >> b >> color >> ilayout >> use;
		d.setId(id);
		d.setPresentationStyle(getPresentationStyle(istyle));
		d.setDisplayType(getDisplayType(itype));
		d.setMaintainOriginalAspectRatio(b);
		d.setBackGroundColor(color);
		d.setStimulusLayoutType(getStimulusLayoutType(ilayout));
		d.setUseISS(use);
	}
	else
	{
		// reset stream to position before trying to read version
		stream.device()->seek(pos);

		// read the rest
		stream >> id >> istyle >> itype >> b >> color;
		d.setId(id);
		d.setPresentationStyle(getPresentationStyle(istyle));
		d.setDisplayType(getDisplayType(itype));
		d.setMaintainOriginalAspectRatio(b);
		d.setBackGroundColor(color);
	}
	return stream;
}

bool Habit::operator==(const Habit::StimulusDisplayInfo& lhs, const Habit::StimulusDisplayInfo& rhs)
{
	return (lhs.getId() == rhs.getId() &&
			lhs.getPresentationStyle() == rhs.getPresentationStyle() &&
			lhs.getDisplayType() == rhs.getDisplayType() &&
			lhs.isOriginalAspectRatioMaintained() == rhs.isOriginalAspectRatioMaintained() &&
			lhs.getBackGroundColor() == rhs.getBackGroundColor() &&
			lhs.getStimulusLayoutType() == rhs.getStimulusLayoutType() &&
			lhs.getUseISS() == rhs.getUseISS());
}


int Habit::StimulusDisplayInfo::getId() const {
	return id_;
}

void Habit::StimulusDisplayInfo::setId(int id) {
	id_ = id;
}

const HPresentationStyle& Habit::StimulusDisplayInfo::getPresentationStyle() const
{
    return *pstyle_;
}

void Habit::StimulusDisplayInfo::setPresentationStyle(const HPresentationStyle& style)
{
    pstyle_ = &style;
}

const HDisplayType& Habit::StimulusDisplayInfo::getDisplayType() const
{
    return *pdtype_;
}

void Habit::StimulusDisplayInfo::setDisplayType(const HDisplayType& dtype)
{
    pdtype_ = &dtype;
}

const HStimulusLayoutType& Habit::StimulusDisplayInfo::getStimulusLayoutType() const
{
	return *playouttype_;
}

void Habit::StimulusDisplayInfo::setStimulusLayoutType(const HStimulusLayoutType& type)
{
	playouttype_ = &type;
}

bool Habit::StimulusDisplayInfo::getUseISS() const
{
	return useISS_;
}

void Habit::StimulusDisplayInfo::setUseISS(bool use)
{
	useISS_ = use;
}

bool Habit::StimulusDisplayInfo::isOriginalAspectRatioMaintained() const
{
    return isOriginalAspectRatioMaintained_;
}

void Habit::StimulusDisplayInfo::setMaintainOriginalAspectRatio(bool maintainOriginalaspectRatio)
{
    isOriginalAspectRatioMaintained_ = maintainOriginalaspectRatio;
}

QColor Habit::StimulusDisplayInfo::getBackGroundColor() const
{
    return backGroundColor_;
}

void Habit::StimulusDisplayInfo::setBackGroundColor(const QColor& backGroundColor)
{
    backGroundColor_ = backGroundColor;
}

void Habit::StimulusDisplayInfo::loadFromDB(int experimentID)
{
	Habit::MainDao dao;
	dao.getStimulusDisplayInfoForExperiment(experimentID, *this);
}

void Habit::StimulusDisplayInfo::saveToDB(int experimentID)
{
	Habit::MainDao dao;
	dao.addOrUpdateStimulusDisplayInfo(experimentID, *this);
}
