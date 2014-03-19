#include "habituationsetupform.h"
#include "habituationsettings.h"
#include "HTypes.h"

#include <QtGui/QGroupBox>
#include <QtGui/QRadioButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QFont>
#include <QtDebug>

namespace GUILib {

const QString 	HabituationSetupForm::FIXED_WINDOW = "Fixed Window";
const QString 	HabituationSetupForm::SLIDING_WINDOW = "Sliding Window";

HabituationSetupForm::HabituationSetupForm(const Habit::HabituationSettings& settings, QWidget *w)
	: QWidget(w)
	, settings_(settings)
{
	createComponents();
	setValidators();
	makeConnections();
	doLayout();
	//setFixedWidth(556);
}

HabituationSetupForm::~HabituationSetupForm()
{
}

void HabituationSetupForm::createComponents()
{
	mainTitle_ = new QLabel(tr("Habituation Setup"));
	habituationGroup_= new QGroupBox(tr("Type of Habituation"));
	habituationCombo_ = new QComboBox(habituationGroup_);
	for (unsigned int i=0; i<sizeof(HHabituationType::A)/sizeof(HHabituationType*); i++)
	{
		const HHabituationType* ap = HHabituationType::A[i];
		habituationCombo_->addItem(ap->label(), ap->number());
	}
	
	basisGroup_ = new QGroupBox(tr("Criterion Basis")); 
	basisRadio1_ = new QRadioButton(tr("Base Criterion on first n trials"));
	basisRadio2_ = new QRadioButton(tr("Base Criterion on longest n trials"));
	percentGroup_ = new QGroupBox(tr("Percent For Criterion")); 
	percentEdit_ = new QLineEdit();
	percentLabel_ = new QLabel(tr("percent"));
	windowMainGroup_ = new QGroupBox(tr("Criterion Window"));
	windowSizeGroup_ =new QGroupBox(tr("Window Size (n)"));
	windowTypeGroup_ = new QGroupBox(tr("Window Type"));
	windowSizeEdit_ = new QLineEdit();
	windowSizeLabel_ = new QLabel(tr("trials"));
	windowTypeCombo_ = new QComboBox(windowTypeGroup_);
	for (unsigned int i=0; i<sizeof(HCriterionWindowType::A)/sizeof(HCriterionWindowType*); i++)
	{
		const HCriterionWindowType* ap = HCriterionWindowType::A[i];
		windowTypeCombo_->addItem(ap->label(), ap->number());
	}
	totalLookLengthToEndGroup_ = new QGroupBox(tr("Total Look Length to End Habituation"));
	totalLookLengthEdit_ = new QLineEdit();
	totalLookLengthLabel_ = new QLabel(tr("ms"));

	// added by djs 3-18-14
	excludeBasisWindowCB_ = new QCheckBox("Exclude basis window when computing percentage");
	requireMinBasisValueCB_ = new QCheckBox("Require minimum basis value");
	minBasisValueEdit_ = new QLineEdit();
	minBasisValueMSLabel_ = new QLabel("ms");

	setLabelsFont();	
}

void HabituationSetupForm::setValidators()
{
	minBasisValueEdit_->setValidator(new QIntValidator(0, 999999, this));
	percentEdit_->setValidator(new QIntValidator(0, 100, this));
	totalLookLengthEdit_->setValidator(new QIntValidator(0, 999999, this));
}


void HabituationSetupForm::setLabelsFont()
{
	QFont font1;
	font1.setPointSize(11);
	habituationGroup_->setFont(font1);
	//habituationCombo_->setGeometry(QRect(140, 0, 101, 22));
	QFont font2;
	font2.setPointSize(9);
	habituationCombo_->setFont(font2);
	windowTypeCombo_->setFont(font2);
	minBasisValueMSLabel_->setFont(font2);
	QFont font3;
	font3.setPointSize(12);
	font3.setBold(true);
	mainTitle_->setFont(font3);
}

void HabituationSetupForm::makeConnections()
{
	connect(habituationCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
	connect(requireMinBasisValueCB_, SIGNAL(toggled(bool)), minBasisValueEdit_, SLOT(setEnabled(bool)));
}

void HabituationSetupForm::doBasisLayout()
{
	QHBoxLayout* hLayout = new QHBoxLayout();
	hLayout->addWidget(requireMinBasisValueCB_);
	hLayout->addWidget(minBasisValueEdit_);
	hLayout->addWidget(minBasisValueMSLabel_);

	QVBoxLayout* basisLayout = new QVBoxLayout();
	basisGroup_->setLayout(basisLayout);
	basisLayout->addWidget(basisRadio1_);
	basisLayout->addWidget(basisRadio2_);
	basisLayout->addStretch(1);
	basisLayout->addLayout(hLayout);
	habituationLayout_->addWidget(basisGroup_);
}

void HabituationSetupForm::doPercentLayout()
{
	QVBoxLayout* percentLayout = new QVBoxLayout();
	QHBoxLayout* hLayout = new QHBoxLayout();

	hLayout->addWidget(percentEdit_);
	hLayout->addWidget(percentLabel_);
	hLayout->addStretch(1);

	percentLayout->addLayout(hLayout);
	percentLayout->addWidget(excludeBasisWindowCB_);

	percentGroup_->setLayout(percentLayout);
	habituationLayout_->addWidget(percentGroup_);
}

void HabituationSetupForm::doWindowLayout()
{
	QHBoxLayout* windowMainLayout = new QHBoxLayout();
	QHBoxLayout* windowSizeLayout = new QHBoxLayout();
	QVBoxLayout* windowTypeLayout = new QVBoxLayout();
	windowMainGroup_->setLayout(windowMainLayout);
	//windowMainLayout->addWidget(habituationCombo_);
	windowMainLayout->addWidget(windowSizeGroup_);
	windowMainLayout->addStretch(1);
	windowMainLayout->addWidget(windowTypeGroup_);
	windowMainLayout->addStretch(1);
	windowSizeGroup_->setLayout(windowSizeLayout);
	windowSizeLayout->addWidget(windowSizeEdit_);
	windowSizeLayout->addWidget(windowSizeLabel_);
	
	windowTypeGroup_->setLayout(windowTypeLayout);
	windowTypeLayout->addWidget(windowTypeCombo_);
	
	habituationLayout_->addWidget(windowMainGroup_);
}

void HabituationSetupForm::doLayout()
{
	habituationLayout_ = new QVBoxLayout();
	habituationGroup_->setLayout(habituationLayout_);
	habituationLayout_->addWidget(habituationCombo_);
	doBasisLayout();
	doPercentLayout();
	doWindowLayout();
	doLookLengthLayout();
	QVBoxLayout* mainLayout =  new QVBoxLayout(this);
	mainLayout->addWidget(mainTitle_);
	mainTitle_->setAlignment(Qt::AlignCenter);
	mainLayout->addWidget(habituationGroup_);
	mainLayout->addStretch(1);
}

void HabituationSetupForm::doLookLengthLayout()
{
	QHBoxLayout* lookLayout = new QHBoxLayout();
	totalLookLengthToEndGroup_->setLayout(lookLayout);
	lookLayout->addWidget(totalLookLengthEdit_);
	lookLayout->addWidget(totalLookLengthLabel_);
	lookLayout->addStretch(1);
	habituationLayout_->addWidget(totalLookLengthToEndGroup_);
}

void HabituationSetupForm::onCurrentIndexChanged(int index)
{
	if (index < 0 || index >= (int)(sizeof(HHabituationType::A)/sizeof(HHabituationType*)))
	{
		qWarning("HabituationSetupForm::onCurrentIndexChanged index(%d) out of range (<%d)", index, (int)(sizeof(HHabituationType::A)/sizeof(HHabituationType*)));
		index = 0;
	}
	if (*HHabituationType::A[index] == HHabituationType::HHabituationTypeFixedN)
	{
		basisGroup_->setVisible(false);
		percentGroup_->setVisible(false);
		windowMainGroup_->setVisible(false);
		totalLookLengthToEndGroup_->setVisible(false);
	} 
	else if (*HHabituationType::A[index] == HHabituationType::HHabituationTypeCriterion)
	{
		basisGroup_->setVisible(true);
		percentGroup_->setVisible(true);
		windowMainGroup_->setVisible(true);
		totalLookLengthToEndGroup_->setVisible(false);
	} 
	else if (*HHabituationType::A[index] == HHabituationType::HHabituationTypeTotalLookingTime)
	{
		basisGroup_->setVisible(false);
		percentGroup_->setVisible(false);
		windowMainGroup_->setVisible(false);
		totalLookLengthToEndGroup_->setVisible(true);
	}
	else 
	{
		qWarning() << "HabituationSetupForm::onCurrentIndexChanged index(" << index << ") type " << HHabituationType::A[index]->name() << " not handled.";
	}
}

void HabituationSetupForm::initialize()
{
	int index;
	index = habituationCombo_->findData(settings_.getHabituationType().number());
	
	if (index == -1)
	{
		qWarning("HabituationSetupForm::initialize() cannot find habituation type combo item with data value %d", settings_.getHabituationType().number());
		index = 0;
	}
	habituationCombo_->setCurrentIndex(index);
	onCurrentIndexChanged(index);
	if (settings_.getCriterionSettings().getBasis() == HCriterionBasisType::HCriterionBasisFirstN)
	{
		basisRadio1_->setChecked(true);
	}
	else if (settings_.getCriterionSettings().getBasis() == HCriterionBasisType::HCriterionBasisLongestN)
	{
		basisRadio2_->setChecked(true);
	}
	percentEdit_->setText(QString("%1").arg(settings_.getCriterionSettings().getPercent()));
	windowSizeEdit_->setText(QString("%1").arg(settings_.getCriterionSettings().getWindowSize()));
	windowTypeCombo_->setCurrentIndex(settings_.getCriterionSettings().getWindowType().number());
	excludeBasisWindowCB_->setChecked(settings_.getCriterionSettings().getExcludeBasisWindow());
	requireMinBasisValueCB_->setChecked(settings_.getCriterionSettings().getRequireMinBasisValue());
	minBasisValueEdit_->setText(QString("%1").arg(settings_.getCriterionSettings().getMinBasisValue()));
	minBasisValueEdit_->setEnabled(settings_.getCriterionSettings().getRequireMinBasisValue());
	totalLookLengthEdit_->setText(QString("%1").arg(settings_.getTotalLookLengthToEnd()));
	index = windowTypeCombo_->findData(settings_.getCriterionSettings().getWindowType().number());
	if (index == -1)
	{
		qWarning("HabituationSetupForm::initialize() cannot find window type combo item with data value %d", settings_.getCriterionSettings().getWindowType().number());
		index = 0;
	}
	windowTypeCombo_->setCurrentIndex(index);
	
}

Habit::HabituationSettings HabituationSetupForm::getConfigurationObject()
{
	Habit::HabituationSettings settings;
	settings.setId(settings_.getId());
	settings.setHabituationType(*HHabituationType::A[habituationCombo_->currentIndex()]);
	Habit::CriterionSettings criterion;
	criterion.setBasis(basisRadio1_->isChecked() ? HCriterionBasisType::HCriterionBasisFirstN : HCriterionBasisType::HCriterionBasisLongestN);
	criterion.setPercent(percentEdit_->text().toInt());
	criterion.setWindowSize(windowSizeEdit_->text().toInt());
	criterion.setWindowType(*HCriterionWindowType::A[windowTypeCombo_->currentIndex()]);
	criterion.setExcludeBasisWindow(excludeBasisWindowCB_->isChecked());
	criterion.setRequireMinBasisValue(requireMinBasisValueCB_->isChecked());
	criterion.setMinBasisValue(minBasisValueEdit_->text().toUInt());
	settings.setCriterionSettings(criterion);
	settings.setTotalLookLengthToEnd(totalLookLengthEdit_->text().toInt());
	return settings;

}

void HabituationSetupForm::setConfigurationObject( const Habit::HabituationSettings& settings)
{
	settings_ = settings;
	initialize();
}

}
