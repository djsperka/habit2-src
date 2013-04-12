#include "stimulussettings.h"

using namespace Habit;

StimulusSettings::StimulusSettings(const QString& name, const HStimContext& context) 
: id_(-1)
, name_(name)
, isLeftEnabled_(false)
, isCenterEnabled_(false)
, isRightEnabled_(false)
, isIndependentSoundEnabled_(false)
, pcontext_(&context)
{};

StimulusSettings::StimulusSettings(const StimulusSettings& s)
: id_(s.getId())
, name_(s.getName())
, isLeftEnabled_(s.isLeftEnabled())
, isCenterEnabled_(s.isCenterEnabled())
, isRightEnabled_(s.isRightEnabled())
, isIndependentSoundEnabled_(s.isIndependentSoundEnabled())
, leftStimulusInfo_(s.getLeftStimulusInfo())
, centerStimulusInfo_(s.getCenterStimulusInfo())
, rightStimulusInfo_(s.getRightStimulusInfo())
, independentSoundInfo_(s.getIndependentSoundInfo())
, pcontext_(s.getContext())
{};


Habit::StimulusSettings::~StimulusSettings()
{}

int Habit::StimulusSettings::getId() const {
	return id_;
}

void Habit::StimulusSettings::setId(int id) {
	id_ = id;
}

QString Habit::StimulusSettings::getName() const 
{
    return name_;
}

void Habit::StimulusSettings::setName(const QString& name)
{
    name_ = name;
}

bool Habit::StimulusSettings::isLeftEnabled() const
{
    return isLeftEnabled_;
}

void Habit::StimulusSettings::setLeftEnabled(bool isLeftEnabled)
{
    isLeftEnabled_ = isLeftEnabled;
}

bool Habit::StimulusSettings::isRightEnabled() const
{
    return isRightEnabled_;
}

void Habit::StimulusSettings::setRightEnabled(bool isRightEnabled)
{
    isRightEnabled_ = isRightEnabled;
}

bool Habit::StimulusSettings::isCenterEnabled() const
{
    return isCenterEnabled_;
}

void Habit::StimulusSettings::setCenterEnabled(bool isCenterEnabled)
{
    isCenterEnabled_ = isCenterEnabled;
}

bool Habit::StimulusSettings::isIndependentSoundEnabled() const
{
    return isIndependentSoundEnabled_;
}

void Habit::StimulusSettings::setIndependentSoundEnabled(bool isIndependentSoundEnabled)
{
    isIndependentSoundEnabled_ = isIndependentSoundEnabled;
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

#if 0
int Habit::StimulusSettings::getStimulusType() const {
	return stimulusType_;
}

void Habit::StimulusSettings::setStimulusType(int type) {
	stimulusType_ = type;
}
#endif

const HStimContext* StimulusSettings::getContext() const
{
	return pcontext_;
}

void StimulusSettings::setContext(const HStimContext& context)
{
	pcontext_ = &context;
}

QDataStream & Habit::operator<< (QDataStream& stream, StimulusSettings settings)
{
	stream << settings.getId();
	stream << settings.getName();
	stream << settings.isLeftEnabled();
	stream << settings.isCenterEnabled();
	stream << settings.isRightEnabled();
	stream << settings.isIndependentSoundEnabled();
	stream << settings.getLeftStimulusInfo();
	stream << settings.getCenterStimulusInfo();
	stream << settings.getRightStimulusInfo();
	stream << settings.getIndependentSoundInfo();
	return stream;
}

QDataStream & Habit::operator>> (QDataStream& stream, StimulusSettings& settings)
{
	int id;
	QString name;
	bool isLeftEnabled;
	bool isCenterEnabled;
	bool isRightEnabled;
	bool isIndependentSoundEnabled;
	StimulusInfo leftStimulusInfo;
	StimulusInfo centerStimulusInfo;
	StimulusInfo rightStimulusInfo;
	StimulusInfo independentSoundInfo;
	int stimulusType;
	stream >> id;
	stream >> name;
	stream >> isLeftEnabled;
	stream >> isCenterEnabled;
	stream >> isRightEnabled;
	stream >> isIndependentSoundEnabled;
	stream >> leftStimulusInfo;
	stream >> centerStimulusInfo;
	stream >> rightStimulusInfo;
	stream >> independentSoundInfo;
	stream >> stimulusType;
	settings.setId(id);
	settings.setName(name);
	settings.setLeftEnabled(isLeftEnabled);
	settings.setCenterEnabled(isCenterEnabled);
	settings.setRightEnabled(isRightEnabled);
	settings.setIndependentSoundEnabled(isIndependentSoundEnabled);
	settings.setLeftStimulusInfo(leftStimulusInfo);
	settings.setCenterStimulusInfo(centerStimulusInfo);
	settings.setRightStimulusInfo(rightStimulusInfo);
	settings.setIndependentSoundInfo(independentSoundInfo);
//	settings.setStimulusType(stimulusType);
	return stream;

}


QDebug Habit::operator<<(QDebug dbg, const StimulusSettings& ss)
{
	dbg.nospace() << "StimulusSettings: Name " << ss.getName() << " id " << ss.getId() << " type " << ss.getContext()->name();
	dbg.nospace() << "                  Left enabled? " << ss.isLeftEnabled();
	if (ss.isLeftEnabled())
		dbg.nospace() << ss.getLeftStimulusInfo();
	dbg.nospace() << "                  Center enabled? " << ss.isCenterEnabled();
	if (ss.isCenterEnabled())
		dbg.nospace() << ss.getCenterStimulusInfo();
	dbg.nospace() << "                  Right enabled? " << ss.isRightEnabled();
	if (ss.isRightEnabled())
		dbg.nospace() << ss.getRightStimulusInfo();
	dbg.nospace() << "                  Independent Sound enabled? " << ss.isIndependentSoundEnabled();
	if (ss.isIndependentSoundEnabled())
		dbg.nospace() << ss.getIndependentSoundInfo();
	return dbg.space();
}


bool Habit::operator==(const Habit::StimulusSettings& lhs, const Habit::StimulusSettings& rhs)
{
	bool bleft, bcenter, bright, bsound;
	bool bother;
	
	if (lhs.isLeftEnabled_)
		bleft = (lhs.isLeftEnabled_ == rhs.isLeftEnabled_ && lhs.leftStimulusInfo_ == rhs.leftStimulusInfo_);
	else 
		bleft = (lhs.isLeftEnabled_ == rhs.isLeftEnabled_);

	if (lhs.isCenterEnabled_)
		bcenter = (lhs.isCenterEnabled_ == rhs.isCenterEnabled_ && lhs.centerStimulusInfo_ == rhs.centerStimulusInfo_);
	else 
		bcenter = (lhs.isCenterEnabled_ == rhs.isCenterEnabled_);

	if (lhs.isRightEnabled_)
		bright = (lhs.isRightEnabled_ == rhs.isRightEnabled_ && lhs.rightStimulusInfo_ == rhs.rightStimulusInfo_);
	else 
		bright = (lhs.isRightEnabled_ == rhs.isRightEnabled_);
	
	if (lhs.isIndependentSoundEnabled_)
		bsound = (lhs.isIndependentSoundEnabled_ == rhs.isIndependentSoundEnabled_ && lhs.independentSoundInfo_ == rhs.independentSoundInfo_);
	else 
		bsound = (lhs.isIndependentSoundEnabled_ == rhs.isIndependentSoundEnabled_);
	
	bother = (lhs.id_ == rhs.id_ &&	lhs.name_ == rhs.name_ && *lhs.getContext() == *rhs.getContext());
	
	return bleft && bcenter && bright && bsound && bother;
}




