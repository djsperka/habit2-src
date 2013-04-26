#include "stimulisettings.h"
#include "maindao.h"

using namespace Habit;

StimuliSettings::StimuliSettings(const HStimContext& context)
: ssList_()
, pcontext_(&context)
{}

StimuliSettings::~StimuliSettings()
{

}

QDataStream & Habit::operator<< (QDataStream& stream, StimuliSettings settings)
{
	stream << settings.getStimuli() << settings.getStimContext().number();
	return stream;
}

QDataStream & Habit::operator>> (QDataStream& stream, StimuliSettings& settings)
{
	Habit::StimulusSettingsList list;
	int icontext;
	stream >> list >> icontext;
	settings.setStimuli(list);
	settings.setStimContext(getStimContext(icontext));
	return stream;
}

bool Habit::operator==(const Habit::StimuliSettings&lhs, const Habit::StimuliSettings& rhs)
{
	return (lhs.getStimContext() == rhs.getStimContext() &&
			lhs.getStimuli() == rhs.getStimuli());
}

StimulusSettingsList StimuliSettings::getStimuli() const
{
    return ssList_;
}

const HStimContext& StimuliSettings::getStimContext() const
{
	return *pcontext_;
}

void StimuliSettings::setStimContext(const HStimContext& context)
{
	pcontext_ = &context;
}

void StimuliSettings::setStimuli(const StimulusSettingsList& stimuli)
{
    ssList_ = stimuli;
}

void StimuliSettings::addStimuli(const StimulusSettings& settings) {
	ssList_.append(settings);
}

void StimuliSettings::loadFromDB(int experimentId) 
{
	MainDao dao;
	if (getStimContext() == HStimContext::PreTestPhase)
	{
		*this = dao.getPretestStimuliSettingsForExperiment(experimentId);
	}
	else if (getStimContext() == HStimContext::HabituationPhase)
	{
		*this = dao.getHabituationStimuliSettingsForExperiment(experimentId);
	}
	else if (getStimContext() == HStimContext::TestPhase)
	{
		*this = dao.getTestStimuliSettingsForExperiment(experimentId);
	}
}

bool Habit::StimuliSettings::saveToDB(size_t id_)
{
	Habit::MainDao dao;
	return dao.addOrUpdateStimuliSetting(id_, this);
}

QDebug Habit::operator<<(QDebug dbg, const StimuliSettings& ss)
{
	StimulusSettingsList c = ss.getStimuli();
	dbg.nospace() << "StimuliSettings: context " << ss.getStimContext().name() << endl;
	for (int i=0; i<c.size(); i++)
	{
		dbg.nospace() << "Stimulus " << i << endl;
		dbg.nospace() << c[i] << endl;
	}
	return dbg.nospace();
}
