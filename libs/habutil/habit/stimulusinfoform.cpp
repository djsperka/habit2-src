#include "stimulusinfoform.h"

#include <QtGui/QCheckBox>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QFileDialog>
#include <QtGui/QSlider>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
#include <QtCore/QFileInfo>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtCore/QSettings>
#include <QtCore/QTDebug>

namespace GUILib {

QString StimulusInfoForm::defaultDir = "";

StimulusInfoForm::StimulusInfoForm(const Habit::StimulusInfo& stim, const QString& pos, bool isIndep, QWidget *w)
	: QWidget(w)
	, stimulusInfo_(stim)
	, stimulusPositionName_(pos)
	, isIndependentStimulus_(isIndep)
{
	createComponents();
	makeConnections();
	doLayout();
}

void StimulusInfoForm::createComponents()
{
	stimulusInfoGroup_ = new QGroupBox(stimulusPositionName_);
	stimulusInfoGroup_->setCheckable(true);
	viewLabel_ = new QLabel();
	fileGroup_ = new QGroupBox(tr("Filename"));
	fileLabel_ = new QLabel(tr(""));
	fileLabel_->setFixedWidth(160);
	fileChooser_ = new QPushButton(tr("&Browse..."));
	loopPlayback_ = new QCheckBox(tr("Loop PlayBack"));
	connect(stimulusInfoGroup_, SIGNAL(toggled(bool)), this, SIGNAL(formSettingsChanged()));
}

bool StimulusInfoForm::isGroupEnabled()
{
	return stimulusInfoGroup_->isChecked();
}

void StimulusInfoForm::setGroupEnabled(bool flag)
{
	emit formSettingsChanged();
	return stimulusInfoGroup_->setChecked(flag);
}

StimulusInfoForm::~StimulusInfoForm()
{
}

void StimulusInfoForm::doFileChooserLayout()
{
}

void StimulusInfoForm::onFileChooserClick()
{
	QFileInfo info(stimulusInfo_.getFileName());
	QString path;
	if (stimulusInfo_.getFileName().isEmpty())
	{
		QSettings settings;
		path = settings.value("lastdir", QString("")).toString();
	}
	else
	{
		path = info.path();
	}
	QString filter;
	if (isIndependentStimulus_)
	{
		filter = "All Audio (*.mp3 *.ogg *.wma *.wav *.aiff);;MPEG Layer 3 Audio (*.mp3);;Ogg Vorbis (*.ogg);;Windows Media Audio (*.wma);;Microsoft Wave (*.wav);;All Files (*.*)";
	}
	else
	{
		filter = "All Video, Audio and Images (*.avi *.mp4 *.wmf *.asf *.wmv *.mov *.mp3 *.ogg *.wma *.wav *.aiff *.bmp *.gif *.png *.jpg *.jpeg *.pbm *.pgm *.ppm *.tif *.tiff *.pict);;All Video (*.avi *.mp4 *.wmf *.asf *.wmv *.mov);;All Audio (*.mp3 *.ogg *.wma *.wav *.aiff);;All Images (*.bmp *.gif *.png *.jpg *.jpeg *.pbm *.pgm *.ppm *.tif *.tiff *.pict);; Audio Video Interleave (*.avi);;MP4 file format (*.mp4);;Windows Media Video (*.wmf *.asf *.wmv);;QuickTime (*.mov);;All Files (*.*)";
	}
	
	QString file = QFileDialog::getOpenFileName(this, "", path, filter);
	if(!file.isEmpty()) {
		QSettings settings;
		QFileInfo ii(file);
		stimulusInfo_.setFileName(file);
		//defaultDir = ii.absoluteDir().absolutePath();
		// save dir in settings
		settings.setValue("lastdir", QString(ii.absoluteDir().absolutePath()));
		QFontMetrics fm = fileLabel_->fontMetrics();
		fileLabel_->setText(fm.elidedText(ii.fileName(), Qt::ElideMiddle, fileLabel_->maximumWidth() ));
	}
	emit formSettingsChanged();
}

void StimulusInfoForm::makeConnections()
{
	connect(fileChooser_, SIGNAL(clicked()), this, SLOT(onFileChooserClick()));
}

void StimulusInfoForm::doIndependentSoundLayout()
{
	mainLayout_->addWidget(stimulusInfoGroup_);
	QVBoxLayout* vbox = new QVBoxLayout(stimulusInfoGroup_);
	QHBoxLayout* hbox = new QHBoxLayout();
	vbox->addLayout(hbox);
	hbox->addWidget(viewLabel_);
	hbox->addWidget(loopPlayback_);
	QVBoxLayout* chooserLayout = new QVBoxLayout();
	fileGroup_->setLayout(chooserLayout);
	chooserLayout->addWidget(fileLabel_);
	chooserLayout->addWidget(fileChooser_);
	vbox->addWidget(fileGroup_);
}

void StimulusInfoForm::doMonitorStimulusLayout()
{
	mainLayout_->addWidget(stimulusInfoGroup_);
	QVBoxLayout* vbox = new QVBoxLayout(stimulusInfoGroup_);
	vbox->addWidget(viewLabel_);
	vbox->setAlignment(viewLabel_,Qt::AlignCenter);
	QVBoxLayout* chooserLayout = new QVBoxLayout();
	fileGroup_->setLayout(chooserLayout);
	chooserLayout->addWidget(fileLabel_);
	chooserLayout->addWidget(fileChooser_);
	vbox->addWidget(fileGroup_);
	vbox->addWidget(loopPlayback_);
}

void StimulusInfoForm::doLayout()
{
	mainLayout_ = new QVBoxLayout(this);
	if(isIndependentStimulus_) {
		doIndependentSoundLayout();
	} else {
		doMonitorStimulusLayout();
	}
}

void StimulusInfoForm::setScreenshot(const QString& path)
{
	QPixmap pixmap(path);
	viewLabel_->setPixmap(pixmap.scaled(60, 60));
	viewLabel_->setFixedSize(60, 60);
}

Habit::StimulusInfo StimulusInfoForm::getConfigurationObject() {
	stimulusInfo_.setLoopPlayBack(loopPlayback_->isChecked());
	return stimulusInfo_;
}

void StimulusInfoForm::initialize(const Habit::StimulusInfo& stimulusInfo) {
	loopPlayback_->setChecked(stimulusInfo.isLoopPlayBack());
	QFileInfo info(stimulusInfo.getFileName());
	QFontMetrics fm = fileLabel_->fontMetrics();
	fileLabel_->setText(fm.elidedText(info.fileName(), Qt::ElideMiddle, fileLabel_->maximumWidth() ));
}

}