#include "attentiongettersettings.h"
#include "maindao.h"

Habit::AttentionGetterSettings::AttentionGetterSettings()
: id_(-1)
, isAttentionGetterUsed_(false)
, attentionGetterStimulus_("", HStimContext::AttentionGetter)
, backGroundColor_(0, 0, 0, 0)
{
}

QDataStream & Habit::operator<< (QDataStream& stream, const AttentionGetterSettings& settings)
{
	stream << settings.getId() << settings.isAttentionGetterUsed() << settings.getAttentionGetterStimulus() << settings.getBackGroundColor();
	return stream;
}

QDataStream & Habit::operator>> (QDataStream& stream, AttentionGetterSettings& settings)
{
	int id;
	bool b;
	StimulusSettings ss;
	QColor color;
	stream >> id >> b >> ss >> color;
	settings.setId(id);
	settings.setUseAttentionGetter(b);
	settings.setAttentionGetterStimulus(ss);
	settings.setBackGroundColor(color);
	return stream;
}

bool Habit::operator==(const Habit::AttentionGetterSettings& lhs, const Habit::AttentionGetterSettings& rhs)
{
	return (lhs.getId() == rhs.getId() &&
			lhs.isAttentionGetterUsed() == rhs.isAttentionGetterUsed() &&
			lhs.getBackGroundColor() == rhs.getBackGroundColor() &&
			lhs.getAttentionGetterStimulus() == rhs.getAttentionGetterStimulus());
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

Habit::StimulusSettings Habit::AttentionGetterSettings::getAttentionGetterStimulus() const
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

void Habit::AttentionGetterSettings::loadFromDB( size_t id )
{
	Habit::MainDao dao;
	dao.getAttentionGetterSettingsForExperiment(id, this);
}

bool Habit::AttentionGetterSettings::saveToDB( size_t id_ )
{
	Habit::MainDao dao;
	return dao.addOrUpdateAttentionGetterSetting(id_, this);
}
