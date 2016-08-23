#include "attentionsetupform.h"
#include "HWorkspaceUtil.h"
#include "HStimulusSettingsEditor.h"
#include <QStringList>
#include <QDir>
#include <QtGui>
#include <QIntValidator>

namespace GUILib {

AttentionSetupForm::AttentionSetupForm(const Habit::AttentionGetterSettings& settings, const Habit::StimulusDisplayInfo& info, QWidget* w)
: QWidget(w)
, m_agSettings(settings)
, m_stimulusDisplayInfo(info)
{
	components();
	connections();
	initialize();
}

AttentionSetupForm::~AttentionSetupForm()
{
}

#if 0
void AttentionSetupForm::setStimulusLayoutType(const HStimulusLayoutType& layoutType)
{
	qDebug() << "Cannot set stimuluslayout type -- see AttentionSetupForm!";
	//m_pLayoutType = &layoutType;
}
#endif

void AttentionSetupForm::stimulusLayoutTypeChanged(int i)
{
	//setStimulusLayoutType(getStimulusLayoutType(i));
	m_pStimulusPreviewWidget->setStimulusLayoutType(getStimulusLayoutType(i));
	m_pStimulusSettingsWidget->setStimulusLayoutType(getStimulusLayoutType(i));
}

void AttentionSetupForm::stimulusSettingsChanged()
{
	// Update previewer
	qDebug() << "stimulus settings changed " << m_pStimulusSettingsWidget->getStimulusSettings();
	m_pStimulusPreviewWidget->clear();
	m_pStimulusPreviewWidget->preview(m_pStimulusSettingsWidget->getStimulusSettings());
}

void AttentionSetupForm::components()
{
	QGroupBox *pIntervalGroup = new QGroupBox(tr("Intertrial Interval"));
	QGroupBox *pStimulusGroup = new QGroupBox(tr("Intertrial Stimulus"));
	m_pStimulusSettingsWidget = new HStimulusSettingsWidget(m_agSettings.getAttentionGetterStimulus(), m_stimulusDisplayInfo.getStimulusLayoutType(), this);

	QDir dir;
	habutilGetStimulusRootDir(dir);

	m_pStimulusPreviewWidget = new HStimulusPreviewWidget(m_stimulusDisplayInfo, dir, this);

	QVBoxLayout* pMainLayout = new QVBoxLayout(this);
	QVBoxLayout* pStimulusLayout = new QVBoxLayout();
	QVBoxLayout* pIntervalLayout = new QVBoxLayout();

	pStimulusGroup->setLayout(pStimulusLayout);
	pStimulusLayout->addWidget(m_pStimulusSettingsWidget);
	pStimulusLayout->addWidget(m_pStimulusPreviewWidget);

	pIntervalGroup->setLayout(pIntervalLayout);
	m_prbNoISI = new QRadioButton("No intertrial interval");
	m_prbNoISI->setToolTip(QString("No gap between trials."));
	m_prbUseAG = new QRadioButton("Use intertrial stimulus as an attention getter");
	m_prbUseAG->setToolTip(QString("Experimenter advances trial with keypress"));
	m_prbUseISI = new QRadioButton("Display intertrial stimulus for a fixed time");
	m_prbUseISI->setToolTip(QString("Trials proceed automatically after time interval"));
	m_plineeditISI = new QLineEdit;
	m_plineeditISI->setValidator(new QIntValidator(1, 999999));
	QLabel *pISI = new QLabel("ms");
	QHBoxLayout *hbox = new QHBoxLayout;
	hbox->addWidget(m_prbUseISI);
	hbox->addStretch(1);
	hbox->addWidget(m_plineeditISI);
	hbox->addStretch(0);
	hbox->addWidget(pISI);

	pIntervalLayout->addWidget(m_prbUseAG);
	pIntervalLayout->addLayout(hbox);
	pIntervalLayout->addWidget(m_prbNoISI);

	pMainLayout->addWidget(pStimulusGroup);
	pMainLayout->addWidget(pIntervalGroup);
	pMainLayout->addStretch(1);
}

void AttentionSetupForm::initialize()
{
	stimulusLayoutTypeChanged(m_stimulusDisplayInfo.getStimulusLayoutType().number());
	m_pStimulusPreviewWidget->preview(m_pStimulusSettingsWidget->getStimulusSettings());
	m_plineeditISI->setText(QString("%1").arg(m_agSettings.getFixedISIMS()));
	m_plineeditISI->setEnabled(false);
	if (m_agSettings.isAttentionGetterUsed())
	{
		m_prbUseAG->setChecked(true);
	}
	else if (m_agSettings.isFixedISI())
	{
		m_prbUseISI->setChecked(true);
		m_plineeditISI->setEnabled(true);
	}
	else
	{
		m_prbNoISI->setChecked(true);
	}
}

Habit::AttentionGetterSettings AttentionSetupForm::getConfigurationObject()
{
	Habit::AttentionGetterSettings ags;
	ags.setId(m_agSettings.getId());
	ags.setUseAttentionGetter(m_prbUseAG->isChecked());
	ags.setIsFixedISI(m_prbUseISI->isChecked());
	ags.setFixedISIMS(m_plineeditISI->text().toInt());
	ags.setAttentionGetterStimulus(m_pStimulusSettingsWidget->getStimulusSettings());
	return ags;
}


void AttentionSetupForm::connections()
{
	//connect(modifyButton_, SIGNAL(clicked()), this, SLOT(onModifyClick()));
	connect(m_pStimulusSettingsWidget, SIGNAL(stimulusSettingsChanged()), this, SLOT(stimulusSettingsChanged()));
	connect(m_prbUseISI, SIGNAL(toggled(bool)), m_plineeditISI, SLOT(setEnabled(bool)));
}

void AttentionSetupForm::setConfigurationObject(const Habit::AttentionGetterSettings& settings)
{
	m_agSettings = settings;
	initialize();
}

}
