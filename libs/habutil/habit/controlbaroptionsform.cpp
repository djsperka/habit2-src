#include "controlbaroptionsform.h"
#include "ui_controlbaroptionsform.h"

using namespace GUILib;

ControlBarOptionsForm::ControlBarOptionsForm(const Habit::ControlBarOptions& controlBarOptions, QWidget *w)
    : QWidget(w)
    , controlBarOptions_(controlBarOptions)
{
    ui = new Ui::ControlBarOptionsFormClass();
    ui->setupUi(this);
    initialize();
	setFixedWidth(556);
}

ControlBarOptionsForm::~ControlBarOptionsForm()
{
    delete ui;
}

void ControlBarOptionsForm::initialize()
{
	ui->displayPhaseCheckBox->setChecked(controlBarOptions_.isCurrentExperimentDisplayed());
	ui->displayStimulusCheckBox->setChecked(controlBarOptions_.isCurrentStimulusDisplayed());
	ui->displayLookingDirectionCheckBox->setChecked(controlBarOptions_.isLookingDirectionDisplayed());
}

Habit::ControlBarOptions ControlBarOptionsForm::getConfigurationObject() {
	Habit::ControlBarOptions cbo;
	cbo.setId(controlBarOptions_.getId());
	cbo.setDisplayCurrentExperiment(ui->displayPhaseCheckBox->isChecked());
	cbo.setDisplayCurrentStimulus(ui->displayStimulusCheckBox->isChecked());
	cbo.setDisplayLookingDirection(ui->displayLookingDirectionCheckBox->isChecked());
	cbo.setUseControlBar(true);
	return cbo;
}

void GUILib::ControlBarOptionsForm::setConfigurationObject( const Habit::ControlBarOptions& settings)
{
	controlBarOptions_ = settings;
	initialize();
}
