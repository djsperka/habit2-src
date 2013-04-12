#include "attentionsetupform.h"
#include "stimulussettingseditordialog.h"

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

AttentionSetupForm::AttentionSetupForm(const Habit::AttentionGetterSettings& settings, QWidget* w)
	: QWidget(w)
	, settings_(settings)
	, dlg_(0)
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

void AttentionSetupForm::createComponents() {
	mainTitle_ = new QLabel(tr("Attention Setup"));
	attentionGroup_ = new QGroupBox(tr("Use Attention Stimulus"));
	attentionGroup_ ->setCheckable(true);
	colorGroup_ = new QGroupBox(tr("Background Color"));
	stimulusName_ = new QLabel("Stimulus Name :  " + settings_.getAttentionGetterStimulus().getName());
	modifyButton_ = new QPushButton(tr("Modify"));
	colorButton_ = new QPushButton();
	colorButton_->setFixedWidth(80);
	colorButton_->setFixedHeight(30);
	setLabelsFont();
	initialize();
}

void AttentionSetupForm::initialize() {
	QPalette p = colorButton_->palette();
	p.setColor(QPalette::Button, settings_.getBackGroundColor());
	colorButton_->setPalette(p);
	QColor c = colorButton_->palette().color(QPalette::Button);
	QString st = QString("background-color: rgb(%1, %2, %3)").arg(c.red()).arg(c.green()).arg(c.blue());
	colorButton_->setStyleSheet(st); 
	attentionGroup_->setChecked(settings_.isAttentionGetterUsed());
	stimulusName_->setText("Stimulus Name :  " + settings_.getAttentionGetterStimulus().getName());
}

Habit::AttentionGetterSettings AttentionSetupForm::getConfigurationObject() {
	Habit::AttentionGetterSettings ags;
	ags.setId(settings_.getId());
	ags.setBackGroundColor(colorButton_->palette().color(QPalette::Button));
	ags.setUseAttentionGetter(attentionGroup_->isChecked());
	ags.setAttentionGetterStimulus(settings_.getAttentionGetterStimulus());
	return ags;
}

void AttentionSetupForm::setLabelsFont()
{
	QFont font1;
	font1.setPointSize(11);
	attentionGroup_->setFont(font1);
	colorGroup_->setFont(font1);
	QFont font2;
	font2.setPointSize(12);
	font2.setBold(true);
	mainTitle_->setFont(font2);
}

void AttentionSetupForm::onColorChooserClick() {
	QColorDialog dlg;
	dlg.setCurrentColor(colorButton_->palette().color(QPalette::Button));
	dlg.exec();
	QColor c = dlg.currentColor();
	QString st = QString("background-color: rgb(%1, %2, %3)").arg(c.red()).arg(c.green()).arg(c.blue());
	colorButton_->setStyleSheet(st); 
}

void AttentionSetupForm::onModifyClick() {
	if(0 == dlg_) {
		dlg_ = new StimulusSettingsEditorDialog(settings_.getAttentionGetterStimulus());	
	}
	else
	{
		dlg_->initialize();
	}
	connect(dlg_, SIGNAL(configurationChanged(Habit::StimulusSettings)), this, SLOT(onConfigurationChange(Habit::StimulusSettings)));
	dlg_->setModal(true);
	dlg_->setVisible(true);
}

void AttentionSetupForm::onConfigurationChange(const Habit::StimulusSettings& stimulus) {
	Q_ASSERT(0 != dlg_);
	settings_.setAttentionGetterStimulus(stimulus);
	stimulusName_->setText("Stimulus Name: " + stimulus.getName());
	dlg_->close();
}

void AttentionSetupForm::makeConnections()
{
	connect(colorButton_, SIGNAL(clicked()), this, SLOT(onColorChooserClick()));
	connect(modifyButton_, SIGNAL(clicked()), this, SLOT(onModifyClick()));
}

void AttentionSetupForm::doAttentionLayout(){
	QVBoxLayout* attentionLayout = new QVBoxLayout();
	attentionGroup_->setLayout(attentionLayout);
	QHBoxLayout* nameLayout = new QHBoxLayout();
	QHBoxLayout* colorLayout = new QHBoxLayout();
	nameLayout->addWidget(stimulusName_);
	nameLayout->addSpacing(70);
	nameLayout->addWidget(modifyButton_);
	attentionLayout->addLayout(nameLayout);
	colorGroup_->setLayout(colorLayout);
	colorLayout->addWidget(colorButton_);
	colorLayout->addStretch();
	attentionLayout->addWidget(colorGroup_);
	mainLayout_->addWidget(attentionGroup_);
	mainLayout_->addStretch(1);
}

void AttentionSetupForm::doLayout()
{
	mainLayout_=  new QVBoxLayout(this);
	mainLayout_->addWidget(mainTitle_);
	mainTitle_->setAlignment(Qt::AlignCenter);
	doAttentionLayout();
}

void AttentionSetupForm::setConfigurationObject(const Habit::AttentionGetterSettings& settings)
{
	settings_ = settings;
	initialize();
}

}