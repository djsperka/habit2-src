
#include "stimulusdisplayform.h"
#include "stimulussettings.h"

#include <QtGui/QGroupBox>
#include <QtGui/QComboBox>
#include <QtGui/QCheckBox>
#include <QtGui/QLabel>
#include <QtGui/QFont>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPalette>
#include <QtGui/QColor>
#include <QtGui/QColorDialog>
#include <QtCore/QTDebug>

namespace GUILib {

const QString StimulusDisplayForm::MONITOR_DEFINED = "Monitor Defined";
const QString StimulusDisplayForm::SINGLE_MOVABLE = "Single Movable";

const QString StimulusDisplayForm::FULL_SCREEN = "Full Screen";
const QString StimulusDisplayForm::ORIGINAL_SIZE = "Original Size";

StimulusDisplayForm::StimulusDisplayForm(const Habit::StimulusDisplayInfo& settings, QWidget* w)
	: QWidget(w)
	, settings_(settings)
{
	createComponents();
	makeConnections();
	doLayout();
	//setFixedWidth(556);
}

StimulusDisplayForm::~StimulusDisplayForm()
{}

void StimulusDisplayForm::createComponents()
{
	mainTitle_ = new QLabel(tr("Stimulus Display"));
	presentationGroup_= new QGroupBox(tr("Presentation Style"));
	presentationCombo_ = new QComboBox(presentationGroup_);
	presentationCombo_->addItem(MONITOR_DEFINED);
	presentationCombo_->addItem(SINGLE_MOVABLE);
	displayGroup_ = new QGroupBox(tr("Display Type")); 
	displayCombo_= new QComboBox(displayGroup_);
	displayCombo_->addItem(FULL_SCREEN);
	displayCombo_->addItem(ORIGINAL_SIZE);
	
	displayCheck_ = new QCheckBox(tr("Maintain Original Aspect Ratio"));
	colorGroup_ = new QGroupBox(tr("Background Color"));
	colorButton_ = new QPushButton();
	colorButton_->setFixedWidth(80);
	colorButton_->setFixedHeight(30);
	setLabelsFont();
	connect(displayCombo_, SIGNAL(currentIndexChanged(const QString& )), this, SLOT(onDisplayComboIndexChanged(const QString&)));
}

void StimulusDisplayForm::onDisplayComboIndexChanged(const QString& index)
{
	displayCheck_->setEnabled(index == FULL_SCREEN);
}

void StimulusDisplayForm::setLabelsFont()
{
	QFont font1;
	font1.setPointSize(11);
	presentationGroup_->setFont(font1);
	colorGroup_->setFont(font1);
	displayGroup_->setFont(font1);
	//presentationCombo_->setGeometry(QRect(129, 0, 101, 22));
	//displayCombo_->setGeometry(QRect(95, 0, 101, 22));
	QFont font2;
	font2.setPointSize(9);
	displayCombo_->setFont(font2);
	presentationCombo_->setFont(font2);
	QFont font3;
	font3.setPointSize(12);
	font3.setBold(true);
	mainTitle_->setFont(font3);
}

void StimulusDisplayForm::makeConnections()
{
	connect(colorButton_, SIGNAL(clicked()), this, SLOT(onColorChooserClick()));
}

void StimulusDisplayForm::doPresentationLayout()
{
	QVBoxLayout* presentationLayout = new QVBoxLayout();
	presentationGroup_->setLayout(presentationLayout);
	presentationLayout->addWidget(presentationCombo_);
	presentationLayout->addWidget(displayGroup_);
	QVBoxLayout* displayLayout = new QVBoxLayout();
	displayGroup_->setLayout(displayLayout);
	displayLayout->addWidget(displayCombo_);
	displayLayout->addWidget(displayCheck_);
	mainLayout_->addWidget(presentationGroup_);
}

void StimulusDisplayForm::doColorLayout()
{
	QHBoxLayout* colorLayout = new QHBoxLayout();
	colorGroup_->setLayout(colorLayout);
	colorLayout->addWidget(colorButton_);
	colorLayout->addStretch();
	mainLayout_->addWidget(colorGroup_);
}

void StimulusDisplayForm::doLayout()
{
	mainLayout_=  new QVBoxLayout(this);
	mainLayout_->addWidget(mainTitle_);
	mainTitle_->setAlignment(Qt::AlignCenter);
	doPresentationLayout();
	doColorLayout();
	mainLayout_->addStretch(1);
}

void StimulusDisplayForm::onColorChooserClick() {
	QColorDialog dlg;
	dlg.setCurrentColor(colorButton_->palette().color(QPalette::Button));
	dlg.exec();
	QColor c = dlg.currentColor();
	QString st = QString("background-color: rgb(%1, %2, %3)").arg(c.red()).arg(c.green()).arg(c.blue());
	colorButton_->setStyleSheet(st); 
}

void StimulusDisplayForm::initialize() {
	presentationCombo_->setCurrentIndex(settings_.getPresentationStyle());
	displayCombo_->setCurrentIndex(settings_.getDisplayType());
	displayCheck_->setChecked(settings_.isOriginalAspectRatioMaintained());
	QPalette p = colorButton_->palette();
	p.setColor(QPalette::Button, settings_.getBackGroundColor());
	colorButton_->setPalette(p);
	QColor c = colorButton_->palette().color(QPalette::Button);
	QString st = QString("background-color: rgb(%1, %2, %3)").arg(c.red()).arg(c.green()).arg(c.blue());
	colorButton_->setStyleSheet(st); 
}

Habit::StimulusDisplayInfo StimulusDisplayForm::getConfigurationObject() {
	Habit::StimulusDisplayInfo settings;
	settings.setId(settings_.getId());
	settings.setPresentationStyle(presentationCombo_->currentText() == MONITOR_DEFINED ? Habit::StimulusDisplayInfo::eMonitorDefined 
																					   : Habit::StimulusDisplayInfo::eSingleMovable);
	settings.setDisplayType(displayCombo_->currentText() == FULL_SCREEN ? Habit::StimulusDisplayInfo::eFullScreen 
																		: Habit::StimulusDisplayInfo::eOriginalSize);
	settings.setMaintainOriginalAspectRatio(displayCheck_->isChecked());
	settings.setBackGroundColor(colorButton_->palette().color(QPalette::Button));
	return settings;
}

void StimulusDisplayForm::setConfigurationObject( const Habit::StimulusDisplayInfo& settings)
{
	settings_ = settings;
	initialize();
}

}