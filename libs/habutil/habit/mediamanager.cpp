#include "mediamanager.h"
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QMessageBox>
#include <QtCore/QDebug>
#include <QtGui/QIcon>

namespace System {

MediaManager::MediaManager(Habit::ExperimentSettings es, QWidget *parent)
	: QWidget(parent),
	streamsCount_(0),
	videoStarted(false),
	esettings_(es),
	leftLabel_(0),
	centerLabel_(0),
	rightLabel_(0),
	controlLabel_(0),
	leftPlayer_(0),
	centerPlayer_(0),
	rightPlayer_(0),
	controlPlayer_(0),
	timer_(0)
{
	Habit::MonitorSettings ms = es.getMonitorSettings();
	bool fullScreen = (es.getStimulusDisplayInfo().getDisplayType() == Habit::StimulusDisplayInfo::eFullScreen);
	bool aspectRatio = (es.getStimulusDisplayInfo().isOriginalAspectRatioMaintained());
	qDebug() << "ms.getLeftMonitorIndex() " << ms.getLeftMonitorIndex();
	if(ms.getLeftMonitorIndex() >= 0) 
	{
		QRect rect = QApplication::desktop()->screenGeometry(ms.getLeftMonitorIndex());
		leftLabel_ = new QLabel();
		Qt::WindowFlags wf = leftLabel_->windowFlags();
		wf |= Qt::WindowStaysOnTopHint;
		leftLabel_->setWindowFlags(wf);
		leftLabel_->setWindowIcon(QIcon(":/resources/analysis.png"));
		leftLabel_->setGeometry(rect);
		leftLabel_->move(rect.x(), rect.y());
		leftPlayer_ = new MediaPlayer(leftLabel_, ms.getLeftMonitorIndex());
		
		// This function will call setFullScreen on the VideoWidgets. The docs for that function say:
		// Warning: When switching to full screen mode using setFullScreen(), the widget onto which the 
		// video is rendered is shown as a top-level window.
		leftPlayer_->setGeometry(rect);
		leftPlayer_->move(rect.x(), rect.y());
		
		leftPlayer_->setWindowMode(fullScreen, aspectRatio);
		connect(leftPlayer_, SIGNAL(playbackFinished()), this, SLOT(onLeftMediaFinished()));
		connect(leftPlayer_, SIGNAL(playTimeFinished()), this, SLOT(onLeftMediaTimeFinished()));
		connect(leftPlayer_, SIGNAL(videoStarted()), this, SLOT(onVideoStarted()));
		qDebug() << "MediaManager::MediaManager - left MediaPlayer ready.";
	}
	qDebug() << "ms.getCenterMonitorIndex() " << ms.getCenterMonitorIndex();
	if(ms.getCenterMonitorIndex() >= 0) 
	{
		QRect rect = QApplication::desktop()->screenGeometry(ms.getCenterMonitorIndex());
		centerLabel_ = new QLabel();
		Qt::WindowFlags wf = centerLabel_->windowFlags();
		wf |= Qt::WindowStaysOnTopHint;
		centerLabel_->setWindowFlags(wf);
		centerLabel_->setWindowIcon(QIcon(":/resources/analysis.png"));
		centerLabel_->setGeometry(rect);
		centerLabel_->move(rect.x(), rect.y());
		centerPlayer_ = new MediaPlayer(centerLabel_, ms.getCenterMonitorIndex());
		centerPlayer_->setWindowMode(fullScreen, aspectRatio);
		connect(centerPlayer_, SIGNAL(playbackFinished()), this, SLOT(onCenterMediaFinished()));
		connect(centerPlayer_, SIGNAL(playTimeFinished()), this, SLOT(onCenterMediaTimeFinished()));
		connect(centerPlayer_, SIGNAL(videoStarted()), this, SLOT(onVideoStarted()));
		qDebug() << "MediaManager::MediaManager - center MediaPlayer ready.";
	}
	qDebug() << "ms.getRightMonitorIndex() " << ms.getRightMonitorIndex();
	if(ms.getRightMonitorIndex() >= 0) 
	{
		QRect rect = QApplication::desktop()->screenGeometry(ms.getRightMonitorIndex());
		rightLabel_ = new QLabel();
		Qt::WindowFlags wf = rightLabel_->windowFlags();
		wf |= Qt::WindowStaysOnTopHint;
		rightLabel_->setWindowFlags(wf);
		rightLabel_->setWindowIcon(QIcon(":/resources/analysis.png"));
		rightLabel_->setGeometry(rect);
		rightLabel_->move(rect.x(), rect.y());
		rightPlayer_ = new MediaPlayer(rightLabel_, ms.getRightMonitorIndex());
		// djs try this

		rightPlayer_->setGeometry(rect);
		rightPlayer_->move(rect.x(), rect.y());
		rightPlayer_->setWindowMode(fullScreen, aspectRatio);
		connect(rightPlayer_, SIGNAL(playbackFinished()), this, SLOT(onRightMediaFinished()));
		connect(rightPlayer_, SIGNAL(playTimeFinished()), this, SLOT(onRightMediaTimeFinished()));
		connect(rightPlayer_, SIGNAL(videoStarted()), this, SLOT(onVideoStarted()));
		qDebug() << "MediaManager::MediaManager - right MediaPlayer ready.";
	}
	if(ms.getControlMonitorIndex() >= 0) 
	{
		controlLabel_ = new QLabel();
		controlLabel_->setGeometry(QRect(0,0,0,0));
		
		controlPlayer_ = new MediaPlayer(controlLabel_, -1);
		connect(controlPlayer_, SIGNAL(playbackFinished()), this, SLOT(onControlMediaFinished()));
		connect(controlPlayer_, SIGNAL(playTimeFinished()), this, SLOT(onControlMediaTimeFinished()));
		qDebug() << "MediaManager::MediaManager - control MediaPlayer ready.";
	}
	qDebug() << "MediaManager::MediaManager - done.";
}

MediaManager::~MediaManager()
{
}

void MediaManager::onVideoStarted()
{
	qDebug() << "videoStarted " << videoStarted;
	if (!videoStarted)
	{
		videoStarted = true;
		qDebug() << "emit playingStarted";
		emit playingStarted();
	}
}

void MediaManager::onLeftMediaFinished()
{
	
	leftPlayer_->stop();
	if (_activessettings.getLeftStimulusInfo().isLoopPlayBack())
	{
		leftPlayer_->prepareToPlay();
		leftPlayer_->playCurrentMedia();

	}
	else
	{
		
		if (!leftFinished)
		{
			leftFinished = true;
			streamsCount_--;
			if (streamsCount_ == 0)
			{
				qDebug() << "onLeftMediaFinished  playbackFinished  EMITED **//**//";
				emit playbackFinished();
			}
			
		}
	}
}

void MediaManager::onCenterMediaFinished()
{
	centerPlayer_->stop();
	if (_activessettings.getCenterStimulusInfo().isLoopPlayBack())
	{
		centerPlayer_->prepareToPlay();
		centerPlayer_->playCurrentMedia();
	}
	else
	{
		
		if (!centerFinished)
		{
			centerFinished = true;
			streamsCount_--;
			if (streamsCount_ == 0)
			{
				qDebug() << "onCenterMediaFinished  playbackFinished  EMITED **//**//";
				emit playbackFinished();
			}
				
		}
	}
}

void MediaManager::onRightMediaFinished()
{
	rightPlayer_->stop();
	
	if (_activessettings.getRightStimulusInfo().isLoopPlayBack())
	{
		rightPlayer_->prepareToPlay();
		rightPlayer_->playCurrentMedia();
	}
	else
	{
		
		if (!rightFinished)
		{
			rightFinished = true;
			streamsCount_--;
			if (streamsCount_ == 0)
			{
				qDebug() << "onRightMediaFinished  playbackFinished  EMITED **//**//";
				emit playbackFinished();
			}
		}
	}
}

void MediaManager::onControlMediaFinished()
{
	controlPlayer_->stop();
	if (_activessettings.getIndependentSoundInfo().isLoopPlayBack())
	{
		controlPlayer_->prepareToPlay();
		controlPlayer_->playCurrentMedia();
	}
	else
	{
		
		if (!controlFinished)
		{
			controlFinished = true;
			streamsCount_--;
			if (streamsCount_ == 0)
			{
				qDebug() << "onControlMediaFinished  playbackFinished  EMITED **//**//";
				emit playbackFinished();
			}
		}
	}
}

void MediaManager::onLeftMediaTimeFinished()
{
	if (leftPlayer_)
	leftPlayer_->stop();
	qDebug() << "Left:" << streamsCount_;
	if (!leftFinished)
	{
		leftFinished = true;
		streamsCount_--;
		if (streamsCount_ == 0)
		{
			qDebug() << "onLeftMediaTimeFinished  playbackFinished  EMITED **//**//";
			emit playbackFinished();
		}
		
	}
}

void MediaManager::onCenterMediaTimeFinished()
{
	if (centerPlayer_)
	centerPlayer_->stop();
	qDebug() << "Center:" << streamsCount_;
	if (!centerFinished)
	{
		centerFinished = true;
	streamsCount_--;
	if (streamsCount_ == 0)
	{
		qDebug() << "onCenterMediaTimeFinished  playbackFinished  EMITED **//**//";
		emit playbackFinished();
	}
	
	}
}

void MediaManager::onRightMediaTimeFinished()
{
	if (rightPlayer_)
	rightPlayer_->stop();
	qDebug() << "Right:" << streamsCount_;
	if (!rightFinished)
	{
		rightFinished = true;
	streamsCount_--;
	if (streamsCount_ == 0)
	{
		qDebug() << "onRightMediaFinished  playbackFinished  EMITED **//**//";
		emit playbackFinished();
	}
	
	}
}

void MediaManager::onControlMediaTimeFinished()
{
	if (controlPlayer_)
	controlPlayer_->stop();
	qDebug() << "Control:" << streamsCount_;
	
	if (!controlFinished)
	{
		controlFinished = true;
		streamsCount_--;
		if (streamsCount_ == 0)
		{
			qDebug() << "onCenterMediaTimeFinished  playbackFinished  EMITED **//**//";
			emit playbackFinished();
		}
		
	}
}

int MediaManager::determineSleepTime(int stimulusType)
{
	int time;
	Habit::DesignSettings ds = esettings_.getDesignSettings();
	switch(stimulusType) {
		case Habit::StimulusSettings::HABITUATION :
			time = ((ds.getHabituationTrialsInfo().getType() == Habit::TrialsInfo::eSubjectControlled) ? -1 : 
				ds.getHabituationTrialsInfo().getLength());
			break;
		case Habit::StimulusSettings::PRETEST :
			time = ((ds.getPretestTrialsInfo().getType() == Habit::TrialsInfo::eSubjectControlled) ? -1 : 
				ds.getPretestTrialsInfo().getLength());
			break;
		case Habit::StimulusSettings::TEST :
			time = ((ds.getTestTrialsInfo().getType() == Habit::TrialsInfo::eSubjectControlled) ? -1 : 
				ds.getTestTrialsInfo().getLength());
			break;
		default:
			time = 0;
	}
	if(time > 0) {
		time *= 100;
	}
	return time;
}

void MediaManager::stop()
{
	qDebug() << "Start stop:" << QTime::currentTime().toString("hh:mm:ss.zzz");
	if (leftPlayer_ && _activessettings.isLeftEnabled())
	{
		leftPlayer_->stop();
		//leftPlayer_->close();
		//leftPlayer_=0;
	}
	if (centerPlayer_ && _activessettings.isCenterEnabled())
	{
		centerPlayer_->stop();
		//centerPlayer_->close();
		//centerPlayer_=0;
	}
	if (rightPlayer_ && _activessettings.isRightEnabled())
	{
		rightPlayer_->stop();
		//rightPlayer_->close();
		//rightPlayer_=0;
	}
	if (controlPlayer_ && _activessettings.isIndependentSoundEnabled())
	{
		controlPlayer_->stop();
		//controlPlayer_->close();
		//controlPlayer_=0;
	}
	qDebug() << "Middle stop:" << QTime::currentTime().toString("hh:mm:ss.zzz");
	
	
	qDebug() << "End stop:" << QTime::currentTime().toString("hh:mm:ss.zzz");
}

void MediaManager::clear()
{
	
	qDebug() << "CLEAR";
	stop();

	QTimer::singleShot(100, this, SLOT(fullclear()));
}

void MediaManager::fullclear()
{
	if (leftLabel_)
	{
		leftLabel_->close();
		delete leftLabel_;
		leftLabel_ = 0;
		
	}
	if (centerLabel_)
	{
		centerLabel_->close();
		delete centerLabel_;
		centerLabel_ = 0;
		
	}
	if (rightLabel_)
	{
		rightLabel_->close();
		delete rightLabel_;
		rightLabel_ = 0;
		
	}
	if (controlLabel_)
	{
		controlLabel_->close();
		delete controlLabel_;
		controlLabel_ = 0;
		
	}
	leftPlayer_=0;
	centerPlayer_=0;
	rightPlayer_=0;
	controlPlayer_=0;
	emit cleared();
}

void MediaManager::onTimeout() 
{
	qDebug() << "ON TIME OUT";
	stop();
}

void MediaManager::loadNext( const Habit::StimulusSettings& ss, QColor backColor )
{
	_backssettings = ss;
	
	_backbackColor = backColor;
	//djs QPalette palette;
	
	qDebug() << "MediaManager::loadNext";
	if(ss.isLeftEnabled() && leftLabel_) 
	{
		qDebug() << "MediaManager::loadNext: leftPlayer_->loadMediaToBuffer(" << _backssettings.getLeftStimulusInfo().getFileName() << ")";
		leftPlayer_->setBackColor(_backbackColor);
		leftPlayer_->loadMediaToBuffer(_backssettings.getLeftStimulusInfo().getFileName());
	}

	if(ss.isCenterEnabled() && centerLabel_) 
	{
		qDebug() << "MediaManager::loadNext: centerPlayer_->loadMediaToBuffer(" << _backssettings.getCenterStimulusInfo().getFileName() << ")";
		centerPlayer_->setBackColor(_backbackColor);
		centerPlayer_->loadMediaToBuffer(_backssettings.getCenterStimulusInfo().getFileName());	
	}

	if(ss.isRightEnabled() && rightLabel_) 
	{
		qDebug() << "MediaManager::loadNext: rightPlayer_->loadMediaToBuffer(" << _backssettings.getRightStimulusInfo().getFileName() << ")";
		rightPlayer_->setBackColor(_backbackColor);
		rightPlayer_->loadMediaToBuffer(_backssettings.getRightStimulusInfo().getFileName());
	}

	if(ss.isIndependentSoundEnabled() && controlLabel_) 
	{
		controlPlayer_->loadMediaToBuffer(_backssettings.getIndependentSoundInfo().getFileName());
	}


	
}

bool MediaManager::play()
{
	videoStarted = false;
	streamsCount_ = 0;
		
	
	if(_activessettings.isLeftEnabled() && leftLabel_) 
	{
		leftPlayer_->setVolume(_activessettings.getLeftStimulusInfo().getAudioBalance());
		//leftPlayer_->setStopTime(sleepTime);
		leftFinished = false;
		streamsCount_++;
		leftLabel_->show();
		//leftLabel_->activateWindow();
		leftPlayer_->prepareToPlay();
		leftPlayer_->setIsLoopBack(_activessettings.getLeftStimulusInfo().isLoopPlayBack());
	}
	else if (leftLabel_)
		leftLabel_->hide();
	if(_activessettings.isCenterEnabled() && centerLabel_) 
	{
		centerPlayer_->setVolume(_activessettings.getCenterStimulusInfo().getAudioBalance());
		//centerPlayer_->setStopTime(sleepTime);
		centerFinished = false;
		streamsCount_++;
		centerLabel_->show();
		//centerLabel_->activateWindow();
		centerPlayer_->prepareToPlay();
		centerPlayer_->setIsLoopBack(_activessettings.getCenterStimulusInfo().isLoopPlayBack());
	}
	else if (centerLabel_)
		centerLabel_->hide();
	if(_activessettings.isRightEnabled() && rightLabel_) 
	{
		rightPlayer_->setVolume(_activessettings.getRightStimulusInfo().getAudioBalance());
		//rightPlayer_->setStopTime(sleepTime);
		rightFinished = false;
		streamsCount_++;
		rightLabel_->show();
		//rightLabel_->activateWindow();
		rightPlayer_->prepareToPlay();
		rightPlayer_->setIsLoopBack(_activessettings.getRightStimulusInfo().isLoopPlayBack());
	}
	else if (rightLabel_)
		rightLabel_->hide();
	if(_activessettings.isIndependentSoundEnabled() && controlLabel_) 
	{
		controlPlayer_->setVolume(_activessettings.getIndependentSoundInfo().getAudioBalance());
		//controlPlayer_->setStopTime(sleepTime);
		controlFinished = false;
		streamsCount_++;
		
		controlPlayer_->prepareToPlay();
		controlPlayer_->setIsLoopBack(_activessettings.getIndependentSoundInfo().isLoopPlayBack());
	}
	else if (controlLabel_)
		controlLabel_->hide();
	if(_activessettings.isLeftEnabled() && leftLabel_) 
	{
		leftPlayer_->playCurrentMedia();
	}
	if(_activessettings.isCenterEnabled() && centerLabel_) 
	{
		centerPlayer_->playCurrentMedia();
	}
	if(_activessettings.isRightEnabled() && rightLabel_) 
	{
		rightPlayer_->playCurrentMedia();
	}
	if(_activessettings.isIndependentSoundEnabled() && controlLabel_) 
	{
		controlPlayer_->playCurrentMedia();
	}
	return true;
}

void MediaManager::switchToNext()
{
	QColor sColor = _activebackColor;
	Habit::StimulusSettings sSettings = _activessettings;
	_activebackColor = _backbackColor;
	_activessettings = _backssettings;
	_backssettings = sSettings;
	_backbackColor = sColor;
	if (leftPlayer_)
		leftPlayer_->swapBuffers();
	if (rightPlayer_)
		rightPlayer_->swapBuffers();
	if (centerPlayer_)
		centerPlayer_->swapBuffers();
	if (controlPlayer_)
		controlPlayer_->swapBuffers();

}

void MediaManager::repeat()
{
	videoStarted = false;
	streamsCount_ = 0;
	if(_activessettings.isLeftEnabled() && leftLabel_) 
	{
		//leftPlayer_->setStopTime(sleepTime);
		leftFinished = false;
		streamsCount_++;
		leftPlayer_->repeatCurrentMedia();
	}
	if(_activessettings.isCenterEnabled() && centerLabel_) 
	{
		//centerPlayer_->setStopTime(sleepTime);
		centerFinished = false;
		streamsCount_++;
		centerPlayer_->repeatCurrentMedia();
	}
	if(_activessettings.isRightEnabled() && rightLabel_) 
	{
		//rightPlayer_->setStopTime(sleepTime);
		rightFinished = false;
		streamsCount_++;
		rightPlayer_->repeatCurrentMedia();
	}
	if(_activessettings.isIndependentSoundEnabled() && controlLabel_) 
	{
		//controlPlayer_->setStopTime(sleepTime);
		controlFinished = false;
		streamsCount_++;
		controlPlayer_->repeatCurrentMedia();
	}
}

void MediaManager::stopForRepeat()
{
	qDebug() << "STOP FOR REPEAT";
	if(_activessettings.isLeftEnabled() && leftLabel_) 
	{
		leftPlayer_->stopCurrentMedia();
	}
	if(_activessettings.isCenterEnabled() && centerLabel_) 
	{
		centerPlayer_->stopCurrentMedia();
	}
	if(_activessettings.isRightEnabled() && rightLabel_) 
	{
		rightPlayer_->stopCurrentMedia();
	}
	if(_activessettings.isIndependentSoundEnabled() && controlLabel_) 
	{
		controlPlayer_->stopCurrentMedia();
	}
}

}