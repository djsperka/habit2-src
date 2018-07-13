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
	QVBoxLayout *vbox = new QVBoxLayout;
	m_pIntertrialIntervalSettingsWidget = new HIntertrialIntervalSettingsWidget(m_agSettings, m_stimulusDisplayInfo, this);
	vbox->addWidget(m_pIntertrialIntervalSettingsWidget);
	m_pStimulusPreviewWidget = new HStimulusPreviewWidget(m_stimulusDisplayInfo, HGMM::instance().getAGKey(), this);
	vbox->addWidget(m_pStimulusPreviewWidget);
	setLayout(vbox);

	connect(m_pIntertrialIntervalSettingsWidget, SIGNAL(stimulusSettingsChanged()), this, SLOT(stimulusSettingsChanged()));
	//initialize();
//	qDebug() << m_agSettings.getAttentionGetterStimulus();
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
	m_pStimulusPreviewWidget->preview(HGMM::instance().getBackgroundKey(), true);
	HGMM::instance().replaceStimulus(HGMM::instance().getAGKey(), m_ssCurrent, m_pIntertrialIntervalSettingsWidget->isSoundOnlyAG());
	m_pStimulusPreviewWidget->preview(HGMM::instance().getAGKey(), true);
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
