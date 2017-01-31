#include "stimulussettings.h"

using namespace Habit;

// version string for input/output. See operator<<, operator>>
static const QString f_sVersion2("SIV2");

StimulusSettings::StimulusSettings()
: id_(-1)
, name_("")
{};

StimulusSettings::StimulusSettings(const StimulusSettings& s)
: id_(s.getId())
, name_(s.getName())
, leftStimulusInfo_(s.getLeftStimulusInfo())
, centerStimulusInfo_(s.getCenterStimulusInfo())
, rightStimulusInfo_(s.getRightStimulusInfo())
, independentSoundInfo_(s.getIndependentSoundInfo())
{};

StimulusSettings& StimulusSettings::operator=(const StimulusSettings& rhs)
{
	if (this != &rhs)
	{
		setId(rhs.getId());
		setName(rhs.getName());
		setLeftStimulusInfo(rhs.getLeftStimulusInfo());
		setCenterStimulusInfo(rhs.getCenterStimulusInfo());
		setRightStimulusInfo(rhs.getRightStimulusInfo());
		setIndependentSoundInfo(rhs.getIndependentSoundInfo());
	}
	return *this;
}

StimulusSettings StimulusSettings::clone() const
{
	StimulusSettings settings(*this);
	settings.setId(-1);
	settings.getLeftStimulusInfo().setId(-1);
	settings.getCenterStimulusInfo().setId(-1);
	settings.getRightStimulusInfo().setId(-1);
	settings.getIndependentSoundInfo().setId(-1);
	return settings;
}

Habit::StimulusSettings::~StimulusSettings()
{}

int Habit::StimulusSettings::getId() const {
	return id_;
}

void Habit::StimulusSettings::setId(int id) {
	id_ = id;
}

const QString& Habit::StimulusSettings::getName() const
{
    return name_;
}

void Habit::StimulusSettings::setName(const QString& name)
{
    name_ = name;
}


Habit::StimulusInfo& Habit::StimulusSettings::getLeftStimulusInfo()
{
    return leftStimulusInfo_;
}

const Habit::StimulusInfo& Habit::StimulusSettings::getLeftStimulusInfo() const
{
    return leftStimulusInfo_;
}

void Habit::StimulusSettings::setLeftStimulusInfo(const StimulusInfo& leftStimulusInfo)
{
    leftStimulusInfo_ = leftStimulusInfo;
}

Habit::StimulusInfo& Habit::StimulusSettings::getCenterStimulusInfo()
{
    return centerStimulusInfo_;
}

const Habit::StimulusInfo& Habit::StimulusSettings::getCenterStimulusInfo() const
{
    return centerStimulusInfo_;
}

void Habit::StimulusSettings::setCenterStimulusInfo(const StimulusInfo& centerStimulusInfo)
{
   centerStimulusInfo_ = centerStimulusInfo;
}

Habit::StimulusInfo& Habit::StimulusSettings::getRightStimulusInfo()
{
    return rightStimulusInfo_;
}

const Habit::StimulusInfo& Habit::StimulusSettings::getRightStimulusInfo() const
{
    return rightStimulusInfo_;
}

void Habit::StimulusSettings::setRightStimulusInfo(const StimulusInfo& rightStimulusInfo)
{
    rightStimulusInfo_ = rightStimulusInfo;
}

Habit::StimulusInfo& Habit::StimulusSettings::getIndependentSoundInfo()
{
    return independentSoundInfo_;
}

const Habit::StimulusInfo& Habit::StimulusSettings::getIndependentSoundInfo() const
{
    return independentSoundInfo_;
}

void Habit::StimulusSettings::setIndependentSoundInfo(const StimulusInfo& independentSoundInfo)
{
    independentSoundInfo_ = independentSoundInfo;
}

void StimulusSettings::setStimulusInfo(const StimulusInfo& info, const HPlayerPositionType& position)
{
	if (position == HPlayerPositionType::Left)
		setLeftStimulusInfo(info);
	else if (position == HPlayerPositionType::Center)
		setCenterStimulusInfo(info);
	else if (position == HPlayerPositionType::Right)
		setRightStimulusInfo(info);
	else if (position == HPlayerPositionType::Sound)
		setIndependentSoundInfo(info);
	else
		qWarning() << "Attempting to set stimulus info for an unknown position: " << position.name();
	return;
}



QDataStream & Habit::operator<< (QDataStream& stream, StimulusSettings settings)
{
	// 3-11-2015, write version number to stream. Layout of StimulusInfo has changed - this will
	// keep the loading of old results from breaking.

	// 4-19-2016, no longer using context, but write a dummy value anyways.

	stream << f_sVersion2 << settings.getId() << settings.getName() <<
			settings.getLeftStimulusInfo() <<
			settings.getCenterStimulusInfo() <<
			settings.getRightStimulusInfo() <<
			settings.getIndependentSoundInfo() << (int)0;
	return stream;
}

QDataStream & Habit::operator>> (QDataStream& stream, StimulusSettings& settings)
{
	int id;
	QString name;
	bool b0, b1, b2, b3;
	StimulusInfo leftStimulusInfo;
	StimulusInfo centerStimulusInfo;
	StimulusInfo rightStimulusInfo;
	StimulusInfo independentSoundInfo;	// there is a dummy var dumped for this; legacy reasons, ignore the value.
	int icontext;
	QString sVersion;
	qint64 pos = stream.device()->pos();

	stream >> sVersion;
	if (sVersion == f_sVersion2)
	{
		// at version 2 the stimulus info a new fields added.
		stream >> id >> name >>
				leftStimulusInfo >> centerStimulusInfo >> rightStimulusInfo >> independentSoundInfo >> icontext;
	}
	else
	{
		StimulusInfoOld oldLeft, oldRight, oldCenter, oldSound;
		stream.device()->seek(pos);
		stream >> id >> name >> b0 >> b1 >> b2 >> b3 >> oldLeft >> oldCenter >> oldRight >> oldSound >> icontext;
		leftStimulusInfo = StimulusInfo(oldLeft.getName(), oldLeft.getFileName(), oldLeft.isLoopPlayBack(), oldLeft.getVolume());
		centerStimulusInfo = StimulusInfo(oldCenter.getName(), oldCenter.getFileName(), oldCenter.isLoopPlayBack(), oldCenter.getVolume());
		rightStimulusInfo = StimulusInfo(oldRight.getName(), oldRight.getFileName(), oldRight.isLoopPlayBack(), oldRight.getVolume());
		independentSoundInfo = StimulusInfo(oldSound.getName(), oldSound.getFileName(), oldSound.isLoopPlayBack(), oldSound.getVolume());
	}
	settings.setId(id);
	settings.setName(name);
	settings.setLeftStimulusInfo(leftStimulusInfo);
	settings.setCenterStimulusInfo(centerStimulusInfo);
	settings.setRightStimulusInfo(rightStimulusInfo);
	settings.setIndependentSoundInfo(independentSoundInfo);
	//settings.setContext(getStimContext(icontext));
	return stream;

}

QTextStream & Habit::operator<< (QTextStream& stream, StimulusSettings settings)
{
	return stream;
}

QTextStream & operator>> (QTextStream& stream, StimulusSettings& settings)
{
	return stream;
}

QDebug Habit::operator<<(QDebug dbg, const StimulusSettings& ss)
{
	dbg.nospace() << "StimulusSettings: Name " << ss.getName() << " id " << ss.getId();
	dbg.nospace() << "Left " << ss.getLeftStimulusInfo();
	dbg.nospace() << "Center " << ss.getCenterStimulusInfo();
	dbg.nospace() << "Right " << ss.getRightStimulusInfo();
	dbg.nospace() << "Sound " << ss.getIndependentSoundInfo();
	return dbg.space();
}


bool Habit::operator==(const Habit::StimulusSettings& lhs, const Habit::StimulusSettings& rhs)
{
	bool bleft, bcenter, bright, bsound;
	bool bother;
	
	bleft = (lhs.leftStimulusInfo_ == rhs.leftStimulusInfo_);
	bcenter = (lhs.centerStimulusInfo_ == rhs.centerStimulusInfo_);
	bright = (lhs.rightStimulusInfo_ == rhs.rightStimulusInfo_);
	bsound = (lhs.independentSoundInfo_ == rhs.independentSoundInfo_);
	
	bother = (lhs.id_ == rhs.id_ &&	lhs.name_ == rhs.name_);

	return bleft && bcenter && bright && bsound && bother;
}




