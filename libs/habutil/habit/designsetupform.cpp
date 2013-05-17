#include "designsetupform.h"
#include "designsettings.h"
#include "trialsinfo.h"

#include <QComboBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFont>
#include <QIntValidator>
#include <QDebug>

namespace GUILib {

#if 0
const QString DesignSetupForm::SUBJECT_CONTROLLED = "Subject Controlled";
const QString DesignSetupForm::FIXED_LENGTH = "Fixed Length";
#endif

DesignSetupForm::DesignSetupForm(const Habit::DesignSettings& settings, QWidget *w)
	: QWidget(w)
	, settings_(settings)
{
	createComponents();
	setValidators();
	makeConnections();
	doLayout();
	//setFixedWidth(556);
}

DesignSetupForm::~DesignSetupForm() {
}

void DesignSetupForm::createComponents() {
	createLookAwayTimes();
	createNumberOfTrials();
	createTrialLength();
	createTrialType();
	setLabelsFont();
}

void DesignSetupForm::createTrialType() {
	trialGroup_ = new QGroupBox(tr("Trial Type"));
	trialPretestGroup_ = new QGroupBox(tr("Pretest"));
	trialHabituationGroup_ = new QGroupBox(tr("Habituation"));
	trialTestGroup_ = new QGroupBox(tr("Test"));
	pretestCombo_ = new QComboBox();
	habituationCombo_ = new QComboBox();
	testCombo_ = new QComboBox();

	//HACK

	qDebug() << "HaCK CRITERIONWindowType " << sizeof(HCriterionWindowType::A)/sizeof(HCriterionWindowType*);
	for (unsigned int i=0; i<sizeof(HCriterionWindowType::A)/sizeof(HCriterionWindowType*); i++)
	{
		const HCriterionWindowType* ap = HCriterionWindowType::A[i];
		qDebug() << "WType combo item " << ap->label() << " " << ap->number();
	}


	const HTrialCompletionType& testtype = HTrialCompletionType::HTrialCompletionSubjectControlled;
	qDebug() << "testtype " << testtype.number() << " " << testtype.label() << " " << testtype.name();
	//ENDHACK


	for (unsigned int i=0; i<sizeof(HTrialCompletionType::A)/sizeof(HTrialCompletionType*); i++)
	{
		const HTrialCompletionType* ap = HTrialCompletionType::A[i];
		qDebug() << "TrialType combo item " << ap->number() << " " << ap->label() << " " << ap->name();
		pretestCombo_->addItem(ap->label(), ap->number());
		habituationCombo_->addItem(ap->label(), ap->number());
		testCombo_->addItem(ap->label(), ap->number());
	}
}

void DesignSetupForm::createTrialLength() {
	trialLengthGroup_ = new QGroupBox(tr("(max) Trial Length"));
	pretestGroup_ = new QGroupBox(tr("Pretest"));
	habituationGroup_ = new QGroupBox(tr("Habituation"));
	testGroup_ = new QGroupBox(tr("Test"));
	pretestEdit_ = new QLineEdit();
	habituationEdit_ = new QLineEdit();
	testEdit_ = new QLineEdit();
}

void DesignSetupForm::createLookAwayTimes() {
	lookTimesGroup_ = new QGroupBox(tr("Look (away) times"));
	minLookTimeGroup_ = new QGroupBox(tr("Minimum Look Time"));
	minLookAwayTimeGroup_ = new QGroupBox(tr("Minimum Look-Away Time"));
	minNoLookTimeGroup_ = new QGroupBox(tr("Maximum No Look Time"));
	minLookTimeEdit_ = new QLineEdit();
	minLookAwayTimeEdit_ = new QLineEdit();
	minNoLookTimeEdit_ = new QLineEdit();
}

void DesignSetupForm::createNumberOfTrials() {
	numberOfTrialsGroup_ = new QGroupBox(tr("Number of Trials"));
	pretestTrialsGroup_ = new QGroupBox(tr("Pretest Trials"));
	habituationTrialsGroup_ = new QGroupBox(tr("Habituation Trials"));
	testTrialsGroup_ = new QGroupBox(tr("Test Trials"));
	pretestTrialsEdit_ = new QLineEdit();
	habituationTrialsEdit_ = new QLineEdit();
	testTrialsEdit_ = new QLineEdit();
}

void DesignSetupForm::setLabelsFont() {
	QFont f;
	f.setPointSize(10);
	trialGroup_->setFont(f);
	trialPretestGroup_->setFont(f);
	trialHabituationGroup_->setFont(f);
	trialTestGroup_->setFont(f);
	trialLengthGroup_->setFont(f);
	pretestGroup_->setFont(f);
	habituationGroup_->setFont(f);
	testGroup_->setFont(f);
	lookTimesGroup_->setFont(f);
	minLookTimeGroup_->setFont(f);
	minLookAwayTimeGroup_->setFont(f);
	minNoLookTimeGroup_->setFont(f);
	numberOfTrialsGroup_->setFont(f);
	pretestTrialsGroup_->setFont(f);
	habituationTrialsGroup_->setFont(f);
	testTrialsGroup_->setFont(f);
}

void DesignSetupForm::setValidators() {
	pretestEdit_->setValidator(new QIntValidator(0, 9999999, this));
	habituationEdit_->setValidator(new QIntValidator(0, 9999999, this));
	testEdit_->setValidator(new QIntValidator(0, 9999999, this));
	minLookAwayTimeEdit_->setValidator(new QIntValidator(0, 9999999, this));
	minNoLookTimeEdit_->setValidator(new QIntValidator(0, 9999999, this));
	minLookTimeEdit_->setValidator(new QIntValidator(0, 9999999, this));
	testTrialsEdit_->setValidator(new QIntValidator(0, 9999999, this));
	pretestTrialsEdit_->setValidator(new QIntValidator(0, 9999999, this));
	habituationTrialsEdit_->setValidator(new QIntValidator(0, 9999999, this));
}

void DesignSetupForm::makeConnections() {
}

void DesignSetupForm::doLayout() {
	doTrialTypeLayout();
	doTrialLengthLayout();
	doLookAwayTimeLayout();
	doNumberOfTrialsLayout();
	QHBoxLayout* mid = new QHBoxLayout();
	mid->addWidget(trialLengthGroup_);
	mid->addWidget(lookTimesGroup_);
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(trialGroup_);
	mainLayout->addLayout(mid);
	mainLayout->addWidget(numberOfTrialsGroup_);
	mainLayout->setSizeConstraint(QLayout::SetMinimumSize);
}

void packBox(QGroupBox* box, QWidget* w) {
	QHBoxLayout* hb = new QHBoxLayout;
	hb->addWidget(w);
	box->setLayout(hb);
}

void packBoxWithLabel(QGroupBox* box, QWidget* w, QLabel* label) {
	QHBoxLayout* hb = new QHBoxLayout;
	hb->addWidget(w);
	hb->addWidget(label);
	box->setLayout(hb);
}

void DesignSetupForm::doTrialTypeLayout() {
	QHBoxLayout* m = new QHBoxLayout(trialGroup_);
	packBox(trialPretestGroup_, pretestCombo_);
	packBox(trialHabituationGroup_, habituationCombo_);
	packBox(trialTestGroup_, testCombo_);
 	m->addWidget(trialPretestGroup_);
 	m->addWidget(trialHabituationGroup_);
 	m->addWidget(trialTestGroup_);
}

void DesignSetupForm::doTrialLengthLayout() {
	QVBoxLayout* m = new QVBoxLayout(trialLengthGroup_);
	packBoxWithLabel(pretestGroup_, pretestEdit_, new QLabel("10ths of a second"));
	packBoxWithLabel(habituationGroup_, habituationEdit_, new QLabel("10ths of a second"));
	packBoxWithLabel(testGroup_, testEdit_, new QLabel("10ths of a second"));
	m->addWidget(pretestGroup_);
	m->addWidget(habituationGroup_);
	m->addWidget(testGroup_);
}

void DesignSetupForm::doLookAwayTimeLayout() {
	QVBoxLayout* m = new QVBoxLayout(lookTimesGroup_);
	packBoxWithLabel(minLookTimeGroup_, minLookTimeEdit_, new QLabel("10ths of a second"));
	packBoxWithLabel(minLookAwayTimeGroup_, minLookAwayTimeEdit_, new QLabel("10ths of a second"));
	packBoxWithLabel(minNoLookTimeGroup_, minNoLookTimeEdit_, new QLabel("10ths of a second"));
	m->addWidget(minLookTimeGroup_);
	m->addWidget(minLookAwayTimeGroup_);
	m->addWidget(minNoLookTimeGroup_);
}

void DesignSetupForm::doNumberOfTrialsLayout() {
	QHBoxLayout* m = new QHBoxLayout(numberOfTrialsGroup_);
	packBox(pretestTrialsGroup_, pretestTrialsEdit_);
	packBox(habituationTrialsGroup_, habituationTrialsEdit_);
	packBox(testTrialsGroup_, testTrialsEdit_);
	m->addWidget(pretestTrialsGroup_);
	m->addWidget(habituationTrialsGroup_);
	m->addWidget(testTrialsGroup_);
}

void DesignSetupForm::initialize() {
	pretestCombo_->setCurrentIndex(settings_.getPretestTrialsInfo().getTrialCompletionType().number());
	habituationCombo_->setCurrentIndex(settings_.getHabituationTrialsInfo().getTrialCompletionType().number());
	testCombo_->setCurrentIndex(settings_.getTestTrialsInfo().getTrialCompletionType().number());
	pretestEdit_->setText(QString("%1").arg(settings_.getPretestTrialsInfo().getLength()));
	habituationEdit_->setText(QString("%1").arg(settings_.getHabituationTrialsInfo().getLength()));
	testEdit_->setText(QString("%1").arg(settings_.getTestTrialsInfo().getLength()));
	minLookTimeEdit_->setText(QString("%1").arg(settings_.getPretestTrialsInfo().getLookTimes()));
	minLookAwayTimeEdit_->setText(QString("%1").arg(settings_.getHabituationTrialsInfo().getLookTimes()));
	minNoLookTimeEdit_->setText(QString("%1").arg(settings_.getTestTrialsInfo().getLookTimes()));
	pretestTrialsEdit_->setText(QString("%1").arg(settings_.getPretestTrialsInfo().getNumberOfTrials()));
	habituationTrialsEdit_->setText(QString("%1").arg(settings_.getHabituationTrialsInfo().getNumberOfTrials()));
	testTrialsEdit_->setText(QString("%1").arg(settings_.getTestTrialsInfo().getNumberOfTrials()));
}

Habit::DesignSettings DesignSetupForm::getConfigurationObject() {
	Habit::DesignSettings ds;
	ds.setId(settings_.getId());
	Habit::TrialsInfo pretest;
	pretest.setTrialCompletionType(*HTrialCompletionType::A[pretestCombo_->currentIndex()]);
	//pretest.setType(pretestCombo_->currentText() == SUBJECT_CONTROLLED ? Habit::TrialsInfo::eSubjectControlled : Habit::TrialsInfo::eFixedLength);
	pretest.setLength(pretestEdit_->text().toUInt());
	pretest.setLookTimes(minLookTimeEdit_->text().toUInt());
	pretest.setNumberOfTrials(pretestTrialsEdit_->text().toUInt());
	Habit::TrialsInfo habituation;
	habituation.setTrialCompletionType(*HTrialCompletionType::A[habituationCombo_->currentIndex()]);
	//habituation.setType(habituationCombo_->currentText() == SUBJECT_CONTROLLED ? Habit::TrialsInfo::eSubjectControlled : Habit::TrialsInfo::eFixedLength);
	habituation.setLength(habituationEdit_->text().toUInt());
	habituation.setLookTimes(minLookAwayTimeEdit_->text().toUInt());
	habituation.setNumberOfTrials(habituationTrialsEdit_->text().toUInt());
	Habit::TrialsInfo test;
	test.setTrialCompletionType(*HTrialCompletionType::A[testCombo_->currentIndex()]);
	//test.setType(testCombo_->currentText() == SUBJECT_CONTROLLED ? Habit::TrialsInfo::eSubjectControlled : Habit::TrialsInfo::eFixedLength);
	test.setLength(testEdit_->text().toUInt());
	test.setLookTimes(minNoLookTimeEdit_->text().toUInt());
	test.setNumberOfTrials(testTrialsEdit_->text().toUInt());
	ds.setPretestTrialsInfo(pretest);
	ds.setHabituationTrialsInfo(habituation);
	ds.setTestTrialsInfo(test);
	return ds;
}

void DesignSetupForm::setConfigurationObject( const Habit::DesignSettings& settings)
{
	settings_ = settings;
	initialize();
}

} // namespace GUILib
