#include "attentiongettersettings.h"
#include "maindao.h"

// version string for input/output. See operator<<, operator>>
static const QString f_sVersion2("AGS2");

Habit::AttentionGetterSettings::AttentionGetterSettings()
: id_(-1)
, isAttentionGetterUsed_(false)
, attentionGetterStimulus_()
, backGroundColor_(0, 0, 0, 0)
, isFixedISI_(false)
, isiMS_(0)
{
}


Habit::AttentionGetterSettings::AttentionGetterSettings(const AttentionGetterSettings& ags)
: id_(ags.getId())
, isAttentionGetterUsed_(ags.isAttentionGetterUsed())
, attentionGetterStimulus_(ags.getAttentionGetterStimulus())
, backGroundColor_(ags.getBackGroundColor())
, isFixedISI_(ags.isFixedISI())
, isiMS_(ags.getFixedISIMS())
{
}

Habit::AttentionGetterSettings& Habit::AttentionGetterSettings::operator=(const Habit::AttentionGetterSettings& rhs)
{
	if (this != &rhs)
	{
		setId(rhs.getId());
		setUseAttentionGetter(rhs.isAttentionGetterUsed());
		setAttentionGetterStimulus(rhs.getAttentionGetterStimulus());
		setBackGroundColor(rhs.getBackGroundColor());
		setIsFixedISI(rhs.isFixedISI());
		setFixedISIMS(rhs.getFixedISIMS());
	}
	return *this;
}

Habit::AttentionGetterSettings Habit::AttentionGetterSettings::clone()
{
	Habit::AttentionGetterSettings settings(*this);
	settings.setId(-1);
	settings.setAttentionGetterStimulus(settings.getAttentionGetterStimulus().clone());
	return settings;
}

bool Habit::AttentionGetterSettings::isFixedISI() const
{
	return isFixedISI_;
}

void Habit::AttentionGetterSettings::setIsFixedISI(bool b)
{
	isFixedISI_ = b;
}

int Habit::AttentionGetterSettings::getFixedISIMS() const
{
	return isiMS_;
}

void Habit::AttentionGetterSettings::setFixedISIMS(int ms)
{
	isiMS_ = ms;
}

QDataStream & Habit::operator<< (QDataStream& stream, const AttentionGetterSettings& settings)
{
	stream << f_sVersion2 << settings.getId() << settings.isAttentionGetterUsed() << settings.getAttentionGetterStimulus() << settings.getBackGroundColor() << settings.isFixedISI() << settings.getFixedISIMS();
	return stream;
}

QDataStream & Habit::operator>> (QDataStream& stream, AttentionGetterSettings& settings)
{
	int id;
	bool b;
	StimulusSettings ss;
	QColor color;
	QString sVersion;
	bool bFixedISI;
	int isiMS;

	// Save position in stream in case this is an old
	qint64 pos = stream.device()->pos();
	stream >> sVersion;
	if (sVersion == f_sVersion2)
	{
		stream >> id >> b >> ss >> color >> bFixedISI >> isiMS;
		settings.setId(id);
		settings.setUseAttentionGetter(b);
		settings.setAttentionGetterStimulus(ss);
		settings.setBackGroundColor(color);
		settings.setIsFixedISI(bFixedISI);
		settings.setFixedISIMS(isiMS);
	}
	else
	{
		// reset stream to position before trying to read version
		stream.device()->seek(pos);

		// now read old-style
		stream >> id >> b >> ss >> color;
		settings.setId(id);
		settings.setUseAttentionGetter(b);
		settings.setAttentionGetterStimulus(ss);
		settings.setBackGroundColor(color);
		settings.setIsFixedISI(false);
		settings.setFixedISIMS(0);
	}
	return stream;
}

bool Habit::operator==(const Habit::AttentionGetterSettings& lhs, const Habit::AttentionGetterSettings& rhs)
{
	return (lhs.getId() == rhs.getId() &&
			lhs.isAttentionGetterUsed() == rhs.isAttentionGetterUsed() &&
			lhs.getBackGroundColor() == rhs.getBackGroundColor() &&
			lhs.getAttentionGetterStimulus() == rhs.getAttentionGetterStimulus() &&
			lhs.isFixedISI() == rhs.isFixedISI() &&
			lhs.getFixedISIMS() == rhs.getFixedISIMS());
}


int Habit::AttentionGetterSettings::getId() const {
	return id_;
}

void Habit::AttentionGetterSettings::setId(int id) {
	id_ = id;
}

Habit::AttentionGetterSettings::~AttentionGetterSettings()
{
}

bool Habit::AttentionGetterSettings::isAttentionGetterUsed() const
{
    return isAttentionGetterUsed_;
}

void Habit::AttentionGetterSettings::setUseAttentionGetter(bool isAttentionGetterUsed)
{
    isAttentionGetterUsed_ = isAttentionGetterUsed;
}

const Habit::StimulusSettings& Habit::AttentionGetterSettings::getAttentionGetterStimulus() const
{
    return attentionGetterStimulus_;
}

Habit::StimulusSettings& Habit::AttentionGetterSettings::getAttentionGetterStimulus()
{
    return attentionGetterStimulus_;
}

void Habit::AttentionGetterSettings::setAttentionGetterStimulus(const Habit::StimulusSettings& attentionGetterStimulus)
{
    attentionGetterStimulus_ = attentionGetterStimulus;
}

QColor Habit::AttentionGetterSettings::getBackGroundColor() const
{
    return backGroundColor_;
}

void Habit::AttentionGetterSettings::setBackGroundColor(const QColor& backGroundColor)
{
    backGroundColor_ = backGroundColor;
}

int Habit::AttentionGetterSettings::getStimulusID() const
{
	return stimulusID_;
}

void Habit::AttentionGetterSettings::setStimulusID(int stimulusID)
{
	stimulusID_ = stimulusID;
}

void Habit::AttentionGetterSettings::loadFromDB(int experimentID)
{
	Habit::MainDao dao;
	Habit::StimulusSettings agss;
	dao.getAttentionGetterSettings(experimentID, *this);
	dao.getStimulusSettings(this->getStimulusID(), agss);
	setAttentionGetterStimulus(agss);
}

void Habit::AttentionGetterSettings::saveToDB(int experimentID)
{
	Habit::MainDao dao;

	// attention getters use phase_id = -1. Their stimulus_id is saved in attention_setup table.
	// Note that we add/update attention_setup table after saving the stimulus (because that record
	// needs the stimulus_id).
	dao.addOrUpdateStimulusSettings(-1, getAttentionGetterStimulus());
	dao.addOrUpdateAttentionGetterSettings(experimentID, *this);

}
