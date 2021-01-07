#include "attentionsetupform.h"
#include "HWorkspaceUtil.h"
#include "HGMM.h"
#include <QStringList>
#include <QDir>
#include <QtGui>
#include <QIntValidator>

namespace GUILib {

AttentionSetupForm::AttentionSetupForm(const Habit::AttentionGetterSettings& settings, const Habit::StimulusDisplayInfo& info, QWidget* w)
: QWidget(w)
, m_agSettings(settings)
, m_ssCurrent(settings.getAttentionGetterStimulus())
, m_stimulusDisplayInfo(info)
{
	m_pIntertrialIntervalSettingsWidget = new HIntertrialIntervalSettingsWidget(m_agSettings, m_stimulusDisplayInfo, this);
	connect(m_pIntertrialIntervalSettingsWidget, SIGNAL(stimulusSettingsChanged()), this, SLOT(stimulusSettingsChanged()));


	m_pStimulusPreviewWidget = new HStimulusPreviewWidget(m_stimulusDisplayInfo, this);


	m_phgmm = new HGMM(info, m_pStimulusPreviewWidget->getStimulusWidgets(), habutilGetStimulusRootDir());
	m_phgmm->addAG(settings.getAttentionGetterStimulus(), settings.isSoundOnly());


	connect(m_pStimulusPreviewWidget, SIGNAL(preroll(unsigned int)), m_phgmm, SLOT(preroll(unsigned int)));
	connect(m_pStimulusPreviewWidget, SIGNAL(pause(unsigned int)), m_phgmm, SLOT(pause(unsigned int)));
	connect(m_pStimulusPreviewWidget, SIGNAL(stim(unsigned int)), m_phgmm, SLOT(stim(unsigned int)));
	connect(m_pStimulusPreviewWidget, SIGNAL(rewind(unsigned int)), m_phgmm, SLOT(rewind(unsigned int)));
	connect(m_pStimulusPreviewWidget, SIGNAL(background()), m_phgmm, SLOT(background()));

	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addWidget(m_pIntertrialIntervalSettingsWidget);
	vbox->addWidget(m_pStimulusPreviewWidget);
	setLayout(vbox);

}

AttentionSetupForm::~AttentionSetupForm()
{
}

void AttentionSetupForm::components()
{
}

void AttentionSetupForm::stimulusSettingsChanged()
{
	m_ssCurrent = m_pIntertrialIntervalSettingsWidget->getCurrentStimulusSettings();
	qDebug() << "AttentionSetupForm::stimulusSettingsChanged() - got stimulus settings:";
	qDebug() << m_ssCurrent;
	m_pStimulusPreviewWidget->preview(m_phgmm->getBackgroundKey(), true);
	m_phgmm->replaceStimulus(m_phgmm->getAGKey(), m_ssCurrent, m_pIntertrialIntervalSettingsWidget->isSoundOnlyAG());
	m_pStimulusPreviewWidget->preview(m_phgmm->getAGKey(), true);
}

void AttentionSetupForm::stimulusDisplayInfoChanged(const Habit::StimulusDisplayInfo& info)
{
	qDebug() << "AttentionSetupForm::stimulusDisplayInfoChanged:";
	qDebug() << info;
}

Habit::AttentionGetterSettings AttentionSetupForm::getAttentionGetterSettings()
{
	Habit::AttentionGetterSettings ags;
	ags = m_pIntertrialIntervalSettingsWidget->getAttentionGetterSettings();
	return ags;
}


}
