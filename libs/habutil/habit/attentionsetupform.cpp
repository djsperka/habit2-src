#include "attentionsetupform.h"
//#include "stimulussettingseditordialog.h"
#include "HStimulusSettingsEditor.h"
#include <QStringList>
#include <QtGui/QWidget>
#include <QtGui/QColorDialog>
#include <QtGui/QFont>
#include <QtGui/QPushButton>
#include <QtGui/QCheckBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QGroupBox>
#include <QtGui/QPalette>

namespace GUILib {

AttentionSetupForm::AttentionSetupForm(const Habit::AttentionGetterSettings& settings, const HStimulusLayoutType& layoutType, QWidget* w)
: QWidget(w)
, settings_(settings)
, m_pLayoutType(&layoutType)
{
	//setFixedWidth(556);
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
	m_pLayoutType = &layoutType;
}

void AttentionSetupForm::stimulusLayoutTypeChanged(int i)
{
	setStimulusLayoutType(getStimulusLayoutType(i));
}


void AttentionSetupForm::createComponents() {
	attentionGroup_ = new QGroupBox(tr("Use Attention Stimulus"));
	attentionGroup_ ->setCheckable(true);
	stimulusName_ = new QLabel("Stimulus Name :  " + settings_.getAttentionGetterStimulus().getName());
	modifyButton_ = new QPushButton(tr("Modify"));
	setLabelsFont();
	initialize();
}

void AttentionSetupForm::initialize()
{
	attentionGroup_->setChecked(settings_.isAttentionGetterUsed());
	stimulusName_->setText("Stimulus Name :  " + settings_.getAttentionGetterStimulus().getName());
}

Habit::AttentionGetterSettings AttentionSetupForm::getConfigurationObject()
{
	Habit::AttentionGetterSettings ags;
	ags.setId(settings_.getId());
	ags.setUseAttentionGetter(attentionGroup_->isChecked());
	ags.setAttentionGetterStimulus(settings_.getAttentionGetterStimulus());
	return ags;
}

void AttentionSetupForm::setLabelsFont()
{
	QFont font1;
	font1.setPointSize(11);
	attentionGroup_->setFont(font1);
	//colorGroup_->setFont(font1);
}

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

void AttentionSetupForm::makeConnections()
{
	connect(modifyButton_, SIGNAL(clicked()), this, SLOT(onModifyClick()));
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
	attentionLayout->addLayout(nameLayout);
	mainLayout_->addWidget(attentionGroup_);
	mainLayout_->addStretch(1);
}

void AttentionSetupForm::setConfigurationObject(const Habit::AttentionGetterSettings& settings)
{
	settings_ = settings;
	initialize();
}

}
