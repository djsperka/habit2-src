#include "habituationstimuliform.h"
#include "habituationstimulitable.h"
#include "stimulussettingseditordialog.h"
#include "maindao.h"

#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QMessageBox>

namespace GUILib {

HabituationStimuliForm::HabituationStimuliForm(const Habit::StimuliSettings& settings, const QString& caption, QWidget *w)
	: QWidget(w)
	, settings_(settings)
	, m_caption(caption)
{
	createComponents();
	setLabelsFont();
	makeConnections();
	doLayout();
}

HabituationStimuliForm::~HabituationStimuliForm()
{
}

void HabituationStimuliForm::createComponents()
{
	mainTitle_ = new QLabel(m_caption);
	modifyStimulus_ = new QPushButton(tr("Modify Stimulus"));
	removeStimulus_ = new QPushButton(tr("Remove Stimulus"));
	newStimulus_ = new QPushButton(tr("New Stimulus"));
	copyStimulusPretest_ = new QPushButton(tr("Copy To Pretest"));
	copyStimulusHabit_ = new QPushButton(tr("Copy To Habituation"));
	copyStimulusTest_ = new QPushButton(tr("Copy To Test"));
	stimuliSettingsTable_ = new HabituationStimuliTable(settings_);
}

void HabituationStimuliForm::setLabelsFont()
{
	QFont font;
	font.setPointSize(12);
	font.setBold(true);
	mainTitle_->setFont(font);
}

void HabituationStimuliForm::onModifyStimulusClick() {
	Habit::StimulusSettings* ss = stimuliSettingsTable_->getSelectedStimulus();
	if(0 != ss){
		StimulusSettingsEditorDialog* dlg = new StimulusSettingsEditorDialog(*ss);
		
		dlg->setModal(true);
		dlg->setVisible(true);
		connect(dlg, SIGNAL(configurationChanged(Habit::StimulusSettings)), this, SLOT(onConfigurationChange(Habit::StimulusSettings)));
	}
}

void HabituationStimuliForm::onRemoveStimulusClick() {
	Habit::StimulusSettings* ss = stimuliSettingsTable_->getSelectedStimulus();
	Habit::MainDao dao;
	QString tableName;
	if (settings_.getStimContext() == HStimContext::HabituationPhase)
	{
		tableName = "habituation_stimuli";
	}
	else if (settings_.getStimContext() == HStimContext::PreTestPhase)
	{
		tableName = "pretest_stimuli";
	}
	else if (settings_.getStimContext() == HStimContext::TestPhase)
	{
		tableName = "test_stimuli";
	}

	if(!tableName.isEmpty()) {
		dao.deleteStimulus(tableName, ss->getId());
	}
	stimuliSettingsTable_->removeSelectedStimulus();
}

void HabituationStimuliForm::onNewStimulusClick() {
	StimulusSettingsEditorDialog* dlg = new StimulusSettingsEditorDialog(Habit::StimulusSettings());
	connect(dlg, SIGNAL(configurationChanged(Habit::StimulusSettings)), this, SLOT(onNewConfiguration(Habit::StimulusSettings)));
	dlg->setModal(true);
	dlg->setVisible(true);
}

void HabituationStimuliForm::onConfigurationChange(const Habit::StimulusSettings& stimulus) {
	StimulusSettingsEditorDialog* dlg = qobject_cast<StimulusSettingsEditorDialog*>(sender());
	Q_ASSERT(0 != dlg);
	if(!stimuliSettingsTable_->updateStimulus(stimulus)) {
		QMessageBox::warning(dlg, "Name exists", "Stimulus named \"" + stimulus.getName() + "\" already exists or the name format is incorrect.");
	} else {
		dlg->close();
	}
}

void HabituationStimuliForm::onNewConfiguration(const Habit::StimulusSettings& stimulus) {
	StimulusSettingsEditorDialog* dlg = qobject_cast<StimulusSettingsEditorDialog*>(sender());
	Q_ASSERT(0 != dlg);
	if(!stimuliSettingsTable_->addNewStimulus(stimulus)) {
		QMessageBox::warning(dlg, "Failed to add Stimulus", "Stimulus named \"" + stimulus.getName() + "\" already exists or the name format is incorrect.");
	} else {
		dlg->close();
	}
}

void HabituationStimuliForm::makeConnections()
{
	connect(modifyStimulus_, SIGNAL(clicked()), this, SLOT(onModifyStimulusClick()));
	connect(removeStimulus_, SIGNAL(clicked()), this, SLOT(onRemoveStimulusClick()));
	connect(newStimulus_, SIGNAL(clicked()), this, SLOT(onNewStimulusClick()));
	connect(copyStimulusPretest_, SIGNAL(clicked()), this, SLOT(onCopyStimulusPretestClick()));
	connect(copyStimulusHabit_, SIGNAL(clicked()), this, SLOT(onCopyStimulusHabitClick()));
	connect(copyStimulusTest_, SIGNAL(clicked()), this, SLOT(onCopyStimulusTestClick()));
}

void HabituationStimuliForm::doButtonsLayout()
{
	QHBoxLayout* buttonsUpLayout = new QHBoxLayout();
	buttonsUpLayout->addWidget(newStimulus_);
	buttonsUpLayout->addWidget(removeStimulus_);
	buttonsUpLayout->addWidget(modifyStimulus_);
	QHBoxLayout* buttonsDownLayout = new QHBoxLayout();
	buttonsDownLayout->addWidget(copyStimulusPretest_);
	buttonsDownLayout->addWidget(copyStimulusHabit_);
	buttonsDownLayout->addWidget(copyStimulusTest_);
	QVBoxLayout* buttonsLayout = new QVBoxLayout();
	buttonsLayout->addLayout(buttonsUpLayout);
	buttonsLayout->addLayout(buttonsDownLayout);
	mainLayout_->addLayout(buttonsLayout);
}

void HabituationStimuliForm::doLayout()
{
	mainLayout_ =  new QVBoxLayout(this);
	mainLayout_->addWidget(mainTitle_);
	mainTitle_->setAlignment(Qt::AlignCenter);
	mainLayout_->addWidget(stimuliSettingsTable_);
	doButtonsLayout();
}

void HabituationStimuliForm::initialize()
{
	stimuliSettingsTable_->initializeTable(settings_); 

}

Habit::StimuliSettings HabituationStimuliForm::getConfigurationObject()
{
	return stimuliSettingsTable_->getConfigurationStimuliObject();
}

void HabituationStimuliForm::setConfigurationObject( const Habit::StimuliSettings& settings)
{
	settings_ = settings;
	initialize();
}

void HabituationStimuliForm::onCopyStimulusClick()
{
	Habit::StimulusSettings* ss = stimuliSettingsTable_->getSelectedStimulus();
	if(0 != ss){
		Habit::StimulusSettings ss_copy(*ss);
		QString name = ss->getName();
		int i = 1;
		while (stimuliSettingsTable_->isStimulusNameExists(name + "_" + QString("%1").arg(i++))) ;
		i--;
		ss_copy.setName(name + "_" + QString("%1").arg(i));
		if(!stimuliSettingsTable_->addNewStimulus(ss_copy)) {
			QMessageBox::warning(NULL, "Failed to add Stimulus", "Stimulus named \"" + ss_copy.getName() + "\" already exists or the name format is incorrect.");
		} 
	}
}

void HabituationStimuliForm::onCopyStimulusPretestClick()
{
	Habit::StimulusSettings* ss = stimuliSettingsTable_->getSelectedStimulus();
	emit copyStimulusToPretest(ss);
}

void HabituationStimuliForm::onCopyStimulusHabitClick()
{
	Habit::StimulusSettings* ss = stimuliSettingsTable_->getSelectedStimulus();
	emit copyStimulusToHabit(ss);
}

void HabituationStimuliForm::onCopyStimulusTestClick()
{
	Habit::StimulusSettings* ss = stimuliSettingsTable_->getSelectedStimulus();
	emit copyStimulusToTest(ss);
}

void HabituationStimuliForm::setAllConfigObjects( const Habit::StimuliSettings& pretest, const Habit::StimuliSettings& habit, const Habit::StimuliSettings& test )
{
	_pretestSettingsTable = new HabituationStimuliTable(pretest);
	_habitSettingsTable = new HabituationStimuliTable(habit);
	_testSettingsTable = new HabituationStimuliTable(test);
}

void HabituationStimuliForm::onCopyStimulus(Habit::StimulusSettings* ss)
{
	if(0 != ss){
		Habit::StimulusSettings ss_copy(*ss);
		ss_copy.setContext(HStimContext::HabituationPhase);
		QString name = ss->getName();
		int i = 1;
		while (stimuliSettingsTable_->isStimulusNameExists(name + "_" + QString("%1").arg(i++))) ;
		i--;

		if(!stimuliSettingsTable_->addNewStimulus(ss_copy)) {
			QMessageBox::warning(NULL, "Failed to add Stimulus", "Stimulus named \"" + ss_copy.getName() + "\" already exists or the name format is incorrect.");
		} 
	}
}

}