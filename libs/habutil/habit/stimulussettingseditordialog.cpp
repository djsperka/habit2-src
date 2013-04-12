
#include "stimulusinfoform.h"
#include "stimulussettingseditordialog.h"
#include "mediaplayer.h"

#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QGroupBox>
#include <QtGui/QComboBox>
#include <QtGui/QSlider>
#include <QtGui/QLineEdit>
#include <QtGui/QMessageBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtCore/QFile>


namespace GUILib {

const QString StimulusSettingsEditorDialog::LEFT_MONITOR = "Left";
const QString StimulusSettingsEditorDialog::CENTER_MONITOR = "Center";
const QString StimulusSettingsEditorDialog::RIGHT_MONITOR = "Right";
const QString StimulusSettingsEditorDialog::INDEPENDENT_SOUND = "Independent sound";
const QString StimulusSettingsEditorDialog::SOUND_PREVIEW_PLAY = "Preview\nsound";
const QString StimulusSettingsEditorDialog::SOUND_PREVIEW_STOP = "Stop";

MediaSoundPreviewPlayer* StimulusSettingsEditorDialog::_mediaSoundPreview = 0;

StimulusSettingsEditorDialog::StimulusSettingsEditorDialog(const Habit::StimulusSettings& ss, QWidget* w)
	: QDialog(w)
	, settings_(ss)
{
	setWindowTitle("Stimulus Settings");
	createComponents();
	makeConnections();
	doLayout();	
	initialize();
}

void StimulusSettingsEditorDialog::createComponents()
{
	nameGroup_ = new QGroupBox(tr("Name"));
	stimulusName_ = new QLineEdit();
	leftEnabled_ = new StimulusInfoForm(settings_.getLeftStimulusInfo(), tr("Left Monitor Stimulus"));
	centerEnabled_ = new StimulusInfoForm(settings_.getCenterStimulusInfo(), tr("Center Monitor Stimulus"));
	rightEnabled_ = new StimulusInfoForm(settings_.getRightStimulusInfo(), tr("Right Monitor Stimulus"));
	independentSoundEnabled_ = new StimulusInfoForm(settings_.getIndependentSoundInfo(), tr("Independent Sound Stimulus"), true);
	createSoundOptionsSection();
	okButton_ = new QPushButton("OK");
	cancelButton_ = new QPushButton("Cancel");
	if (!_mediaSoundPreview)
	_mediaSoundPreview = new MediaSoundPreviewPlayer(0);
}

void StimulusSettingsEditorDialog::createSoundOptionsSection()
{
	soundOptionsGroup_ = new QGroupBox(tr("Sound Options"));
	previewSound_ = new QPushButton(SOUND_PREVIEW_PLAY);
	audioBalanceGroup_ = new QGroupBox(tr("Volume"));
	
	audioBalanceLeft_ = new QSlider();
	audioBalanceLeft_->setOrientation(Qt::Vertical);
	audioBalanceLeft_->setRange(0, 100);
	audioBalanceLeft_->setTickPosition(QSlider::TicksRight);
	audioBalanceLeft_->setSingleStep(1);
	audioBalanceLeft_->setValue(50);
	audioBalanceLeft_->setTracking(false);

	audioBalanceRight_ = new QSlider();
	audioBalanceRight_->setOrientation(Qt::Vertical);
	audioBalanceRight_->setRange(0, 100);
	audioBalanceRight_->setTickPosition(QSlider::TicksRight);
	audioBalanceRight_->setSingleStep(1);
	audioBalanceRight_->setValue(50);
	audioBalanceRight_->setTracking(false);

	audioBalanceCenter_ = new QSlider();
	audioBalanceCenter_->setOrientation(Qt::Vertical);
	audioBalanceCenter_->setRange(0, 100);
	audioBalanceCenter_->setTickPosition(QSlider::TicksRight);
	audioBalanceCenter_->setSingleStep(1);
	audioBalanceCenter_->setValue(50);
	audioBalanceCenter_->setTracking(false);

	audioBalanceISS_ = new QSlider();
	audioBalanceISS_->setOrientation(Qt::Vertical);
	audioBalanceISS_->setRange(0, 100);
	audioBalanceISS_->setTickPosition(QSlider::TicksRight);
	audioBalanceISS_->setSingleStep(1);
	audioBalanceISS_->setValue(50);
	audioBalanceISS_->setTracking(false);
}

void StimulusSettingsEditorDialog::doSoundOptionsLayout()
{
	QHBoxLayout* audioBalanceLayout = new QHBoxLayout();
	soundOptionsGroup_->setLayout(audioBalanceLayout);
	QVBoxLayout* soundLayout = new QVBoxLayout();
	audioBalanceLayout->addLayout(soundLayout);
	soundLayout->addWidget(previewSound_,1);
	audioBalanceLayout->addWidget(audioBalanceGroup_,4);
	QHBoxLayout* audioSliderLayout= new QHBoxLayout(audioBalanceGroup_);
	QVBoxLayout* leftLayout = new QVBoxLayout(audioBalanceGroup_);
	leftLayout->addWidget(audioBalanceLeft_); leftLayout->addWidget(new QLabel("Left", audioBalanceGroup_));
	QVBoxLayout* centerLayout = new QVBoxLayout(audioBalanceGroup_);
	centerLayout->addWidget(audioBalanceCenter_); centerLayout->addWidget(new QLabel("Center", audioBalanceGroup_));
	QVBoxLayout* rightLayout = new QVBoxLayout(audioBalanceGroup_);
	rightLayout->addWidget(audioBalanceRight_); rightLayout->addWidget(new QLabel("Right", audioBalanceGroup_));
	QVBoxLayout* issLayout = new QVBoxLayout(audioBalanceGroup_);
	issLayout->addWidget(audioBalanceISS_); issLayout->addWidget(new QLabel("ISS", audioBalanceGroup_));
	audioSliderLayout->addStretch(1);
	audioSliderLayout->addLayout(leftLayout);
	audioSliderLayout->addStretch(1);
	audioSliderLayout->addLayout(centerLayout);
	audioSliderLayout->addStretch(1);
	audioSliderLayout->addLayout(rightLayout);
	audioSliderLayout->addStretch(1);
	audioSliderLayout->addLayout(issLayout);
	audioSliderLayout->addStretch(1);
}

void StimulusSettingsEditorDialog::doLayout()
{
	mainLayout_ = new QVBoxLayout(this);
	mainLayout_->addWidget(nameGroup_);
	QHBoxLayout* editLayout = new QHBoxLayout();
	editLayout->addWidget(stimulusName_);
	editLayout->addStretch();
	nameGroup_->setLayout(editLayout);
	QGridLayout* grid = new QGridLayout();
	grid->addWidget(leftEnabled_, 0, 0);
	grid->addWidget(centerEnabled_, 0, 1);
	grid->addWidget(rightEnabled_, 0, 2);
	grid->addWidget(independentSoundEnabled_, 1, 0);
	grid->addWidget(soundOptionsGroup_, 1, 1, -1, 2);
	doSoundOptionsLayout();
	QHBoxLayout* buttonsLayout = new QHBoxLayout();
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(okButton_);
	buttonsLayout->addWidget(cancelButton_);
	mainLayout_->addLayout(grid);
	mainLayout_->addLayout(buttonsLayout);
}

StimulusSettingsEditorDialog::~StimulusSettingsEditorDialog()
{}

void StimulusSettingsEditorDialog::fillConfigurationObject() {
	previewSound_->setText(SOUND_PREVIEW_PLAY);
	_mediaSoundPreview->stopMedias();
	QString name = stimulusName_->text();
	if(validate()) {
		settings_.setName(name);
		settings_.setLeftEnabled(leftEnabled_->isGroupEnabled());
		settings_.setCenterEnabled(centerEnabled_->isGroupEnabled());
		settings_.setRightEnabled(rightEnabled_->isGroupEnabled());
		settings_.setIndependentSoundEnabled(independentSoundEnabled_->isGroupEnabled());
		Habit::StimulusInfo leftInfo = leftEnabled_->getConfigurationObject();
		Habit::StimulusInfo centerInfo = centerEnabled_->getConfigurationObject();
		Habit::StimulusInfo rightInfo = rightEnabled_->getConfigurationObject();
		Habit::StimulusInfo independentSoundInfo = independentSoundEnabled_->getConfigurationObject();
		leftInfo.setAudioBalance(settings_.getLeftStimulusInfo().getAudioBalance());
		centerInfo.setAudioBalance(settings_.getCenterStimulusInfo().getAudioBalance());
		rightInfo.setAudioBalance(settings_.getRightStimulusInfo().getAudioBalance());
		independentSoundInfo.setAudioBalance(settings_.getIndependentSoundInfo().getAudioBalance());
		settings_.setLeftStimulusInfo(leftInfo);
		settings_.setCenterStimulusInfo(centerInfo);
		settings_.setRightStimulusInfo(rightInfo);
		settings_.setIndependentSoundInfo(independentSoundInfo);
		initializeSettingsAudioBalance();
		emit configurationChanged(settings_);
	}
}

bool StimulusSettingsEditorDialog::validate() 
{
	bool t = false;
	if(stimulusName_->text().trimmed().isEmpty()) {
		QMessageBox::warning(this, "Name is empty", "Name field must be set");
	} else
	if(!leftEnabled_->isGroupEnabled() && !centerEnabled_->isGroupEnabled() && 
		!rightEnabled_->isGroupEnabled() && !independentSoundEnabled_->isGroupEnabled()) {
		QMessageBox::warning(this, "No configuration chosen", "Selection of at least one configuration is required");
	} else
	if(leftEnabled_->isGroupEnabled() && leftEnabled_->getConfigurationObject().getFileName().isEmpty()) {
		QMessageBox::warning(this, "Left media-file is not chosen", "Please select media-file for left monitor configuration");
	} else
	if(centerEnabled_->isGroupEnabled() && centerEnabled_->getConfigurationObject().getFileName().isEmpty()) {
		QMessageBox::warning(this, "Center media-file is not chosen", "Please select media-file for center monitor configuration");
	} else
	if(rightEnabled_->isGroupEnabled() && rightEnabled_->getConfigurationObject().getFileName().isEmpty()) {
		QMessageBox::warning(this, "Right media-file is not chosen", "Please select media-file for right monitor configuration");
	} else
	if(independentSoundEnabled_->isGroupEnabled() && independentSoundEnabled_->getConfigurationObject().getFileName().isEmpty()) {
		QMessageBox::warning(this, "Independent sound media-file is not chosen", "Please select media-file for independent configuration");
	} else {
		t = true;
	}
	return t;
}

void StimulusSettingsEditorDialog::initialize() 
{
	stimulusName_->setText(settings_.getName());
	leftEnabled_->setGroupEnabled(settings_.isLeftEnabled());
	rightEnabled_->setGroupEnabled(settings_.isRightEnabled());
	centerEnabled_->setGroupEnabled(settings_.isCenterEnabled());
	independentSoundEnabled_->setGroupEnabled(settings_.isIndependentSoundEnabled());
	leftEnabled_->initialize(settings_.getLeftStimulusInfo());
	centerEnabled_->initialize(settings_.getCenterStimulusInfo());
	rightEnabled_->initialize(settings_.getRightStimulusInfo());
	independentSoundEnabled_->initialize(settings_.getIndependentSoundInfo());
	if (!isAnyMediaExists())
	{
		independentSoundEnabled_->setGroupEnabled(false);
		soundOptionsGroup_->setEnabled(false);
		independentSoundEnabled_->setDisabled(true);
	}
	else
	{
		soundOptionsGroup_->setEnabled(true);
		independentSoundEnabled_->setDisabled(false);
	}
	initializeAudioBalance();
	connect(leftEnabled_, SIGNAL(formSettingsChanged()), this, SLOT(onFormSettingsChanged()));
	connect(rightEnabled_, SIGNAL(formSettingsChanged()), this, SLOT(onFormSettingsChanged()));
	connect(centerEnabled_, SIGNAL(formSettingsChanged()), this, SLOT(onFormSettingsChanged()));
}

void StimulusSettingsEditorDialog::onFormSettingsChanged()
{
	if (!isAnyMediaExists())
	{
		independentSoundEnabled_->setGroupEnabled(false);
		independentSoundEnabled_->setDisabled(true);
		soundOptionsGroup_->setEnabled(false);
	}
	else
	{
		soundOptionsGroup_->setEnabled(true);
		independentSoundEnabled_->setDisabled(false);
	}
}

bool StimulusSettingsEditorDialog::isAnyMediaExists()
{
	bool isAnyEnabled = (leftEnabled_->isGroupEnabled() && !leftEnabled_->getConfigurationObject().getFileName().isEmpty())
		|| (rightEnabled_->isGroupEnabled() && !rightEnabled_->getConfigurationObject().getFileName().isEmpty()) 
		|| (centerEnabled_->isGroupEnabled() && !centerEnabled_->getConfigurationObject().getFileName().isEmpty()) ;
	return isAnyEnabled;
}

void StimulusSettingsEditorDialog::setStimulus(const Habit::StimulusSettings& stimulus) 
{
	settings_ = stimulus;
	initialize();
}

Habit::StimulusSettings StimulusSettingsEditorDialog::getStimulus() {
	return settings_;
}

void StimulusSettingsEditorDialog::makeConnections()
{
	connect(okButton_, SIGNAL(clicked()), this, SLOT(fillConfigurationObject()));
	connect(cancelButton_, SIGNAL(clicked()), this, SLOT(onClose()));
	connect(previewSound_, SIGNAL(clicked()), this, SLOT(onSoundPreview()));
	connect(audioBalanceLeft_, SIGNAL(valueChanged(int)), this, SLOT(onLeftSliderValueChanged(int)));
	connect(audioBalanceCenter_, SIGNAL(valueChanged(int)), this, SLOT(onCenterSliderValueChanged(int)));
	connect(audioBalanceRight_, SIGNAL(valueChanged(int)), this, SLOT(onRightSliderValueChanged(int)));
	connect(audioBalanceISS_, SIGNAL(valueChanged(int)), this, SLOT(onISSSliderValueChanged(int)));
}

void StimulusSettingsEditorDialog::onLeftSliderValueChanged(int vol) 
{
	settings_.getLeftStimulusInfo().setAudioBalance(vol);
}

void StimulusSettingsEditorDialog::onCenterSliderValueChanged(int vol) 
{
	settings_.getCenterStimulusInfo().setAudioBalance(vol);
}

void StimulusSettingsEditorDialog::onRightSliderValueChanged(int vol) 
{
	settings_.getRightStimulusInfo().setAudioBalance(vol);
}

void StimulusSettingsEditorDialog::onISSSliderValueChanged(int vol) 
{
	settings_.getIndependentSoundInfo().setAudioBalance(vol);
}
	
void StimulusSettingsEditorDialog::initializeSettingsAudioBalance() {
		_mediaSoundPreview->setVolumes(audioBalanceLeft_->value(), audioBalanceCenter_->value(), audioBalanceRight_->value(), audioBalanceISS_->value());
		settings_.getLeftStimulusInfo().setAudioBalance(audioBalanceLeft_->value());
		settings_.getCenterStimulusInfo().setAudioBalance(audioBalanceCenter_->value());
		settings_.getRightStimulusInfo().setAudioBalance(audioBalanceRight_->value());
		settings_.getIndependentSoundInfo().setAudioBalance(audioBalanceISS_->value());
}

void StimulusSettingsEditorDialog::initializeAudioBalance() {
	audioBalanceLeft_->setValue(settings_.getLeftStimulusInfo().getAudioBalance());
	audioBalanceCenter_->setValue(settings_.getCenterStimulusInfo().getAudioBalance());
	audioBalanceRight_->setValue(settings_.getRightStimulusInfo().getAudioBalance());
	audioBalanceISS_->setValue(settings_.getIndependentSoundInfo().getAudioBalance());
}

void StimulusSettingsEditorDialog::onSoundPreview() {
	if(previewSound_->text() == SOUND_PREVIEW_PLAY) {
		previewSound_->setText(SOUND_PREVIEW_STOP);
		_mediaSoundPreview->setMedias(
			(leftEnabled_->isGroupEnabled())?(leftEnabled_->getConfigurationObject().getFileName()):"",
			(centerEnabled_->isGroupEnabled())?(centerEnabled_->getConfigurationObject().getFileName()):"",
			(rightEnabled_->isGroupEnabled())?(rightEnabled_->getConfigurationObject().getFileName()):"",
			(independentSoundEnabled_->isGroupEnabled())?(independentSoundEnabled_->getConfigurationObject().getFileName()):""
			);
		_mediaSoundPreview->setVolumes(audioBalanceLeft_->value(), audioBalanceCenter_->value(), audioBalanceRight_->value(), audioBalanceISS_->value());
		_mediaSoundPreview->playMedias();
	} else {
		previewSound_->setText(SOUND_PREVIEW_PLAY);
		_mediaSoundPreview->stopMedias();
	}
}

void StimulusSettingsEditorDialog::onPlaybackFinished() {
	qDebug() << "STIMULUS SETTINGS EDITOR - ON PLAYBACK FINISHED";
}

void StimulusSettingsEditorDialog::onClose() {
	previewSound_->setText(SOUND_PREVIEW_PLAY);
	_mediaSoundPreview->stopMedias();
	close();
}

} // namespace GUILib