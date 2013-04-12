#include "stimulisettings.h"
#include "maindao.h"

using namespace Habit;

StimuliSettings::StimuliSettings(const HStimContext& context)
: stimuliSettings_()
, pcontext_(&context)
{}

StimuliSettings::~StimuliSettings()
{

}

StimuliSettings::stimulus_container Habit::StimuliSettings::getStimuli() const 
{
    return stimuliSettings_;
}

const HStimContext& StimuliSettings::getStimContext() const
{
	return *pcontext_;
}


void StimuliSettings::setStimuli(const Habit::StimuliSettings::stimulus_container& stimuli)
{
    stimuliSettings_ = stimuli;
}

void StimuliSettings::addStimuli(const StimulusSettings& settings) {
	stimuliSettings_.append(settings);
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
	QVector<Habit::StimulusSettings> c = ss.getStimuli();
	dbg.nospace() << "StimuliSettings: context " << ss.getStimContext().name() << endl;
	for (int i=0; i<c.size(); i++)
	{
		dbg.nospace() << "Stimulus " << i << endl;
		dbg.nospace() << c[i] << endl;
	}
	return dbg.nospace();
}
