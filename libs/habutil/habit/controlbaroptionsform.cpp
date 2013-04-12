#include "controlbaroptionsform.h"
#include "ui_controlbaroptionsform.h"

using namespace GUILib;

ControlBarOptionsForm::ControlBarOptionsForm(const Habit::ControlBarOptions& controlBarOptions, QWidget *w)
    : QWidget(w)
    , controlBarOptions_(controlBarOptions)
{
    ui = new Ui::ControlBarOptionsFormClass();
    ui->setupUi(this);
    //QObject::connect(ui->useControlBarGroupBox, SIGNAL(toggled(bool)), this,  SLOT(useControlBar(bool)));
    //QObject::connect(ui->displayPhaseCheckBox, SIGNAL(stateChanged(int)), this,  SLOT(displayExperimentPhase(int)));
    //QObject::connect(ui->displayStimulusCheckBox, SIGNAL(stateChanged(int)), this,  SLOT(displayStimulus(int)));
	setFixedWidth(556);
}

ControlBarOptionsForm::~ControlBarOptionsForm()
{
    delete ui;
}

void ControlBarOptionsForm::initialize() {
	ui->useControlBarGroupBox->setChecked(controlBarOptions_.isControlBarUsed());
	ui->displayPhaseCheckBox->setChecked(controlBarOptions_.isCurrentExperimentDisplayed());
	ui->displayStimulusCheckBox->setChecked(controlBarOptions_.isCurrentStimulusDisplayed());
}

Habit::ControlBarOptions ControlBarOptionsForm::getConfigurationObject() {
	Habit::ControlBarOptions cbo;
	cbo.setId(controlBarOptions_.getId());
	cbo.setDisplayCurrentExperiment(ui->displayPhaseCheckBox->isChecked());
	cbo.setDisplayCurrentStimulus(ui->displayStimulusCheckBox->isChecked());
	cbo.setUseControlBar(ui->useControlBarGroupBox->isChecked());
	return cbo;
}

void GUILib::ControlBarOptionsForm::setConfigurationObject( const Habit::ControlBarOptions& settings)
{
	controlBarOptions_ = settings;
	initialize();
}
