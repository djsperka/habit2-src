#include "attentionsetupform.h"
#include "HWorkspaceUtil.h"
#include "HStimulusSettingsEditor.h"
#include <QStringList>
#include <QDir>
#include <QtGui>

namespace GUILib {

AttentionSetupForm::AttentionSetupForm(const Habit::AttentionGetterSettings& settings, const Habit::StimulusDisplayInfo& info, QWidget* w)
: QWidget(w)
, settings_(settings)
, m_stimulusDisplayInfo(info)
{
	createComponents();
	makeConnections();
	doLayout();
	initialize();
}

AttentionSetupForm::~AttentionSetupForm()
{
}

void AttentionSetupForm::setStimulusLayoutType(const HStimulusLayoutType& layoutType)
{
	qDebug() << "Cannot set stimuluslayout type -- see AttentionSetupForm!";
	//m_pLayoutType = &layoutType;
}

void AttentionSetupForm::stimulusLayoutTypeChanged(int i)
{
	setStimulusLayoutType(getStimulusLayoutType(i));
}

void AttentionSetupForm::stimulusSettingsChanged()
{
	// Update previewer
	qDebug() << "stimulus settings changed " << stimulusSettingsWidget_->getStimulusSettings();
	stimulusPreviewWidget_->clear();
	stimulusPreviewWidget_->preview(stimulusSettingsWidget_->getStimulusSettings());
}

void AttentionSetupForm::createComponents() {
	attentionGroup_ = new QGroupBox(tr("Use Attention Stimulus"));
	attentionGroup_ ->setCheckable(true);
	stimulusName_ = new QLabel("Stimulus Name :  " + settings_.getAttentionGetterStimulus().getName());
	modifyButton_ = new QPushButton(tr("Modify"));
	stimulusSettingsWidget_ = new HStimulusSettingsWidget(settings_.getAttentionGetterStimulus(), m_stimulusDisplayInfo.getStimulusLayoutType(), this);

	QDir dir;
	habutilGetStimulusRootDir(dir);

	stimulusPreviewWidget_ = new HStimulusPreviewWidget(m_stimulusDisplayInfo, dir, this);
	setLabelsFont();
	initialize();
}

void AttentionSetupForm::initialize()
{
	attentionGroup_->setChecked(settings_.isAttentionGetterUsed());
	//stimulusName_->setText("Stimulus Name :  " + settings_.getAttentionGetterStimulus().getName());
	stimulusPreviewWidget_->preview(stimulusSettingsWidget_->getStimulusSettings());
}

Habit::AttentionGetterSettings AttentionSetupForm::getConfigurationObject()
{
	Habit::AttentionGetterSettings ags;
	ags.setId(settings_.getId());
	ags.setUseAttentionGetter(attentionGroup_->isChecked());
//	ags.setAttentionGetterStimulus(settings_.getAttentionGetterStimulus());
	ags.setAttentionGetterStimulus(stimulusSettingsWidget_->getStimulusSettings());
	return ags;
}

void AttentionSetupForm::setLabelsFont()
{
	QFont font1;
	font1.setPointSize(11);
	attentionGroup_->setFont(font1);
	//colorGroup_->setFont(font1);
}

/*
void AttentionSetupForm::onModifyClick()
{
	QStringList empty;	// dummy empty name list for HStimulusSettingsEditor
	HStimulusSettingsEditor *pEditor = new HStimulusSettingsEditor(settings_.getAttentionGetterStimulus(), *m_pLayoutType, empty, QString("Modify Attention Getter Stimulus"), this);
	if (pEditor->exec() == QDialog::Accepted)
	{
		// update the AG stimulus settings and label
		settings_.setAttentionGetterStimulus(pEditor->getStimulusSettings());
		stimulusName_->setText(QString("Stimulus Name :  " + settings_.getAttentionGetterStimulus().getName()));
	}
}
*/

void AttentionSetupForm::makeConnections()
{
	//connect(modifyButton_, SIGNAL(clicked()), this, SLOT(onModifyClick()));
	connect(stimulusSettingsWidget_, SIGNAL(stimulusSettingsChanged()), this, SLOT(stimulusSettingsChanged()));
}

void AttentionSetupForm::doLayout()
{
	mainLayout_=  new QVBoxLayout(this);
	QVBoxLayout* attentionLayout = new QVBoxLayout();
	attentionGroup_->setLayout(attentionLayout);
	QHBoxLayout* nameLayout = new QHBoxLayout();
	nameLayout->addWidget(stimulusName_);
	nameLayout->addSpacing(70);
	nameLayout->addWidget(modifyButton_);
	//djsattentionLayout->addLayout(nameLayout);
	attentionLayout->addWidget(stimulusSettingsWidget_);
	attentionLayout->addWidget(stimulusPreviewWidget_);
	mainLayout_->addWidget(attentionGroup_);
	mainLayout_->addStretch(1);
}

void AttentionSetupForm::setConfigurationObject(const Habit::AttentionGetterSettings& settings)
{
	settings_ = settings;
	initialize();
}

}
