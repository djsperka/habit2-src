#include "mediaplayer.h"
#include "sleeper.h"

#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtCore/QTimer>
#include <QtCore/QString>
#include <Phonon/MediaObject>
#include <Phonon/VideoWidget>
#include <Phonon/AudioOutput>
#include <QtGui/QLabel>
#include <QtCore/QFile>
#include <QtGui/QMessageBox>
#include <QtCore/QTDebug>
#include <QtCore/QFileInfo>
#include <QtGui/QImage>

namespace System {
	
void MediaPlayer::dumpGeometry(const QString& label, QWidget* w)
{	
	qDebug() << label << " " << w->x() << ", " << w->y() << " " << w->width() << "x" << w->height();
	return;
}

MediaPlayer::MediaPlayer(QLabel* label, int monitorId, QWidget* w) 
	: QWidget(w)
	, isPlaying_(false)
	, isImage_(false)
	, _isFullScreen(false)
	, _isAspectRatio(false)
	, startImageTime_()
	, _mediaSwitcherFlag(false)
	, _currentMediaObject(0)
	, _firstMediaObject(0)
	, _secondMediaObject(0)
	, _currentVideoWidget(0)
	, _firstVideoWidget(0)
	, _secondVideoWidget(0)
	, parent_(0)
	, _currentImageWgt(0)
	, _firstImageWgt(0)
	, _secondImageWgt(0)
	, _currentAudioOutput(0)
	, _firstAudioOutput(0)
	, _secondAudioOutput(0)
	, volume_(0)
	, monitorId_(monitorId)
	, _previousFile("")
	, _waitForPlayed(false)
{
	parent_ = label;
	if (parent_ )
	{
		
		// Creates two MediaObjects, two VideoWidgets and two ImageWidgets. 
		// By maintaining a pointer to the "current" ones this class seems to be implementing a double-buffering 
		// scheme of sorts. . . .  
		// The parent_ widget is a QLabel, and the video/image widgets are drawn on top of it. Some control over the 
		// parent is exercised here, but not total control. This is confusing in places, because sometimes the label
		// is manipulated here, and other times it is manipulated elsewhere. 
		
	 	_currentMediaObject = _firstMediaObject = new Phonon::MediaObject(label);
		_currentVideoWidget = _firstVideoWidget = new Phonon::VideoWidget(label);
		_currentImageWgt = _firstImageWgt	= new System::ImageWidget(label);
		_firstImageWgt->setGeometry(label->geometry());
		_secondMediaObject = new Phonon::MediaObject(label);
		_secondVideoWidget = new Phonon::VideoWidget(label);
		_secondImageWgt	= new System::ImageWidget(label);
		_secondImageWgt->setGeometry(label->geometry());
		parent_->setAutoFillBackground(true);
		_firstImageWgt->setAutoFillBackground(true);
		_secondImageWgt->setAutoFillBackground(true);
		
		// Create paths for phonon to play video and sound.
		Phonon::createPath(_firstMediaObject, _firstVideoWidget);
		_currentAudioOutput = _firstAudioOutput = new Phonon::AudioOutput(Phonon::VideoCategory, this);
		Phonon::createPath(_firstMediaObject, _firstAudioOutput);
		connect(_firstMediaObject, SIGNAL(finished()), this, SLOT(onFirstMediaFinished()));
		connect(_firstMediaObject, SIGNAL(hasVideoChanged(bool)), this, SLOT(onFirstHasVideo(bool)));
		connect(_firstMediaObject, SIGNAL(stateChanged (Phonon::State, Phonon::State)), this, SLOT(onFirstStateChanged(Phonon::State, Phonon::State)));  
		connect(_firstMediaObject, SIGNAL(prefinishMarkReached(qint32)), this, SLOT(onFirstAboutToFinish(qint32)));

		Phonon::createPath(_secondMediaObject, _secondVideoWidget);
		_secondAudioOutput = new Phonon::AudioOutput(Phonon::VideoCategory, this);
		Phonon::createPath(_secondMediaObject, _secondAudioOutput);
		connect(_secondMediaObject, SIGNAL(finished()), this, SLOT(onSecondMediaFinished()));
		connect(_secondMediaObject, SIGNAL(hasVideoChanged(bool)), this, SLOT(onSecondHasVideo(bool)));
		connect(_secondMediaObject, SIGNAL(stateChanged (Phonon::State, Phonon::State)), this, SLOT(onSecondStateChanged(Phonon::State, Phonon::State)));  
		connect(_secondMediaObject, SIGNAL(prefinishMarkReached(qint32)), this, SLOT(onSecondAboutToFinish(qint32)));
	}
	else
	{
		_currentMediaObject = new Phonon::MediaObject(this);
		_currentVideoWidget = new Phonon::VideoWidget(this);
		Phonon::createPath(_currentMediaObject, _currentVideoWidget);
		_currentAudioOutput = new Phonon::AudioOutput(Phonon::VideoCategory, this);
		Phonon::createPath(_currentMediaObject, _currentAudioOutput);
		connect(_currentMediaObject, SIGNAL(finished()), this, SLOT(onCurrentMediaFinished()));
	}

}

MediaPlayer::~MediaPlayer() {
}

void MediaPlayer::setWindowMode(bool fullScreen, bool aspectRatio)
{
	if (!parent_)
		return;
	_isFullScreen = fullScreen;
	_isAspectRatio = aspectRatio;
	
	_firstVideoWidget->setScaleMode(Phonon::VideoWidget::FitInView);
	if (aspectRatio)
	{
		_firstVideoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioAuto);
	}
	else
	{
		_firstVideoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioWidget);
	}
	_secondVideoWidget->setScaleMode(Phonon::VideoWidget::FitInView);
	if (aspectRatio)
	{
		_secondVideoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioAuto);
	}
	else
	{
		_secondVideoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioWidget);
	}
	dumpGeometry("First video widget, before setFullScreen", _firstVideoWidget);
	_firstVideoWidget->setFullScreen(fullScreen);
	dumpGeometry("First video widget, after setFullScreen", _firstVideoWidget);
	_secondVideoWidget->setFullScreen(fullScreen);
}

bool MediaPlayer::isImage(const QString& fileName)
{
	QFileInfo fileInfo(fileName);
	QString suffix = fileInfo.suffix().toUpper();
	return (suffix == "JPG") ||
			(suffix == "JPEG") ||
			(suffix == "PNG") ||
			(suffix == "GIF") ||
			(suffix == "TIF") ||
			(suffix == "TIFF") ||
			(suffix == "PPM") ||
			(suffix == "PGM") ||
			(suffix == "PBM") ||
			(suffix == "PNM") ||
			(suffix == "BMP") ||
			(suffix == "TGA") ||
			(suffix == "PICT") ;

}

void MediaPlayer::onHasVideo(bool hasVideo)
{
	Q_UNUSED(hasVideo);
}

void MediaPlayer::onStateChanged(Phonon::State newstate, Phonon::State oldstate) 
{
	Q_UNUSED(oldstate);
	qDebug() << "MediaPlayer::onStateChanged:" << newstate;
	if (newstate == Phonon::PlayingState)
	{
		qDebug() << "started media" << QTime::currentTime().toString("hh:mm:ss.zzz");
		emit videoStarted();
		
		if (!_currentVideoWidget->isFullScreen())
		{
			QSize size = _currentVideoWidget->sizeHint();
			qDebug() << "VideoWidget sizeHint: " << size.width() << ":" << size.height();
			_currentVideoWidget->setGeometry((parent_->width() - size.width())/2,
				(parent_->height() - size.height())/2,
				size.width(),
				size.height() );
		}
	}
}

bool MediaPlayer::playMedia(QFile* f, bool fullScreen) {
	bool result = false;
	if(f->exists()) {
		if(0 != f) {
			_currentMediaObject->setCurrentSource(f);
			_currentAudioOutput->setVolume(volume_);
			_currentMediaObject->play();
			_currentVideoWidget->setFullScreen(fullScreen);
			result = _currentMediaObject->errorType() == Phonon::NoError;
		}
	}
	return result;
}

void MediaPlayer::stop() {

	//timer_->stop();
	if(0 != _currentMediaObject) {
		_currentMediaObject->stop();
		_currentVideoWidget->hide();
	}
	if (0 != _currentImageWgt)
	{
		_currentImageWgt->hide();
	}
}

void MediaPlayer::playStop() {
	if (0 != _currentMediaObject)
		_currentMediaObject->stop();
}

void MediaPlayer::onMediaFinished() {
	qDebug() << "MediaFinished:" << _currentMediaObject->currentTime()+1;
	qDebug() << "Media Finished:" << QTime::currentTime().toString("hh:mm:ss.zzz");
	//timer_->stop();
	//mediaObject->clear();
	qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz");
	if (stopTime_ > 0)
	{
		stopTime_ -= (_currentMediaObject->currentTime()+1);
		if (stopTime_ < 0)
		{
			qDebug() << "EMIT PLAY_TIME_FINISHED";
			isPlaying_ = false;
			emit playTimeFinished();
		}
		else
		{
			qDebug() << "EMIT PLAYBACK_FINISHED stopTime_ > 0;";
			isPlaying_ = false;
			emit playbackFinished();
		}
	}
	else
	{
		qDebug() << "EMIT PLAYBACK_FINISHED stopTime_ < 0;";
		isPlaying_ = false;
		emit playbackFinished();
	}
}

void MediaPlayer::setStopTime(int stimTime)
{
	stopTime_ = /*100 * */stimTime;
}

void MediaPlayer::onTimeout() {
	if ((stopTime_) > 0)
	{
		if (isImage_)
		{
			int msec = startImageTime_.msecsTo(QTime::currentTime());
			if (msec > stopTime_)
			{
				emit playTimeFinished();
				return;
			}
		}
		else
		if (_currentMediaObject->state() == Phonon::PlayingState)
		{
			if (_currentMediaObject->currentTime() > (stopTime_))
			{
				qDebug() << _currentMediaObject->currentTime() << this;
				qDebug() << stopTime_;
				if (isPlaying_ ) 
				{
					isPlaying_ = false;
					emit playTimeFinished();
					return;
				}	
			}
		}
	}
	if (_currentMediaObject->state() != Phonon::StoppedState || isImage_)
	{
		//timer_->start(10);
	}
}


void MediaPlayer::setVolume(int vol) {
	volume_ = vol;
	if (_currentAudioOutput)
		_currentAudioOutput->setVolume(volume_/100.0);
}

void MediaPlayer::onCurrentMediaFinished()
{
	qDebug() << "START ON CURRENT MEDIA FINISHED";
	onMediaFinished();
	qDebug() << "END ON CURRENT MEDIA FINISHED";
}

void MediaPlayer::onFirstMediaFinished()
{
	qDebug() << "ON FIRST MEDIA FINISHED **************";
	onMediaFinished();
}

void MediaPlayer::onFirstHasVideo(  bool )
{
	
}

void MediaPlayer::onFirstStateChanged(  Phonon::State newstate,  Phonon::State oldstate )
{
	Q_UNUSED(oldstate);
	qDebug() << "MediaPlayer::onFirstStateChanged:" << newstate;
	if (newstate == Phonon::PlayingState && _waitForPlayed)
	{
		_waitForPlayed = false;
		qDebug() << "started media" << QTime::currentTime().toString("hh:mm:ss.zzz");
		emit videoStarted();

		if (!_firstVideoWidget->isFullScreen())
		{
			QSize size = _firstVideoWidget->sizeHint();
			qDebug() << "VideoWidget sizeHint: " << size.width() << ":" << size.height();
			_firstVideoWidget->setGeometry((parent_->width() - size.width())/2,
				(parent_->height() - size.height())/2,
				size.width(),
				size.height() );
		}
	}
}

void MediaPlayer::onSecondMediaFinished()
{
	qDebug() << "ON SECOND MEDIA FINISHED *********";
	onMediaFinished();
}

void MediaPlayer::onSecondHasVideo( bool )
{
	
}

void MediaPlayer::onSecondStateChanged( Phonon::State newstate, Phonon::State oldstate )
{
	Q_UNUSED(oldstate);
	qDebug() << "MediaPlayer::onSecondStateChanged:" << newstate << "  " << Phonon::PlayingState << "  " << _waitForPlayed;
	if ((newstate == Phonon::PlayingState) && _waitForPlayed)
	{
		qDebug() << "newstate == Phonon::PlayingState CONDITION SATISFIED";
		_waitForPlayed = false;
		qDebug() << "started media" << QTime::currentTime().toString("hh:mm:ss.zzz");
		emit videoStarted();

		if (!_secondVideoWidget->isFullScreen())
		{
			QSize size = _secondVideoWidget->sizeHint();
			qDebug() << "VideoWidget sizeHint: " << size.width() << ":" << size.height();
			_secondVideoWidget->setGeometry((parent_->width() - size.width())/2,
				(parent_->height() - size.height())/2,
				size.width(),
				size.height() );
		}
	}
}

void MediaPlayer::loadMediaToBuffer( const QString& file )
{
	qDebug() << "MediaPlayer::loadMediaToBuffer(" << file << ")";
	if (_mediaSwitcherFlag)
	{
		qDebug() << "_mediaSwitcherFlag";
		_firstMediaObject->clear();
		_firstMediaObject->setCurrentSource(file);
		qDebug() << "MediaPLayer::loadMediaToBuffer: firstMediaObject->setCurrentSource(file); " << file;
	}
	else
	{
		qDebug() << "!!_mediaSwitcherFlag";
		_secondMediaObject->clear();
		_secondMediaObject->setCurrentSource(file);
		qDebug() << "MediaPLayer::loadMediaToBuffer: secondMediaObject->setCurrentSource(file); " << file;
	}
}

void MediaPlayer::prepareToPlay()
{
	//if (_mediaSwitcherFlag)
	//{
	QString fileName = _currentMediaObject->currentSource().fileName();
	qDebug() << "MediaPlayer::prepareToPlay()";
	if (isImage(fileName))
	{
		QRect rect = QApplication::desktop()->screenGeometry(monitorId_);
#ifndef Q_OS_MAC
		if (monitorId_==0)
		{
		/*	rect.setX(rect.x() + rect.width()/2);
			rect.setY(rect.y() + rect.height()/2);
			rect.setWidth(10);
			rect.setHeight(10);*/
		}
#endif
		//djs parent_->setGeometry(rect);
		//djs parent_->showFullScreen();
		_currentImageWgt->setGeometry(QRect(0,0,parent_->width(), parent_->height()));
		_currentVideoWidget->hide();
	} 
	else
	{
		_currentMediaObject->seek(0);
		_currentMediaObject->setPrefinishMark(100);
		_currentAudioOutput->setVolume(volume_ / 100.0);
		_currentImageWgt->hide();

		if (monitorId_ >= 0)
		{
			QRect rect = QApplication::desktop()->screenGeometry(monitorId_);
	#ifndef Q_OS_MAC
	/*		if (monitorId_ == 0)
			{
				rect.setX(rect.x() + rect.width()/2);
				rect.setY(rect.y() + rect.height()/2);
				rect.setWidth(10);
				rect.setHeight(10);
			}*/
	#endif
			parent_->setGeometry(rect);
			parent_->showFullScreen();
			_currentVideoWidget->setGeometry(parent_->geometry());

		}
		_currentVideoWidget->show();
		_currentVideoWidget->setFullScreen(_isFullScreen);
	}
	qDebug() << "MediaPlayer::prepareToPlay() - done.";
	
}

void MediaPlayer::playCurrentMedia()
{
	bool result = false;
	QString fileName = _currentMediaObject->currentSource().fileName();
	if (isImage(fileName))
	{
		if (!_currentImageWgt->setFileName(fileName, _isFullScreen, _isAspectRatio))
		{
			result = false;
		}
		else
		{
			_currentImageWgt->show();
			result = true;
			isImage_ = true;
			emit videoStarted();
			startImageTime_ = QTime::currentTime();
			//timer_->start(10);
		}
	}
	else
	{
		isImage_ = false;
		//timer_->start(10);
		_waitForPlayed = true;
		_currentMediaObject->play();
		isPlaying_ = true;
	}
	qDebug() << "start play " << QTime::currentTime().toString("hh:mm:ss.zzz");
}

void MediaPlayer::swapBuffers()
{
	// Implement the double-buffering scheme here. 
	// The current video and image widgets are hidden, then 
	// the notion of _current is changed. The var _mediaSwitcherFlag (bool) 
	// is used to track which objects are "current". 
	
	_currentVideoWidget->hide();
	_currentImageWgt->hide();
	_mediaSwitcherFlag = !_mediaSwitcherFlag;
	if (_mediaSwitcherFlag)
	{
		_currentAudioOutput	= _secondAudioOutput;
		_currentImageWgt	= _secondImageWgt;
		_currentMediaObject	= _secondMediaObject;
		_currentVideoWidget	= _secondVideoWidget;
	} 
	else
	{
		_currentAudioOutput	= _firstAudioOutput;
		_currentImageWgt	= _firstImageWgt;
		_currentMediaObject	= _firstMediaObject;
		_currentVideoWidget	= _firstVideoWidget;
	}
}

void MediaPlayer::setBackColor( QColor backColor )
{
	QPalette palette;
	palette.setColor(QPalette::Active, QPalette::Window, backColor);
	palette.setColor(QPalette::Inactive, QPalette::Window, backColor);
	palette.setColor(QPalette::Active, QPalette::Base, backColor);
	palette.setColor(QPalette::Inactive, QPalette::Base, backColor);
	palette.setColor(QPalette::Active, QPalette::WindowText, backColor);
	palette.setColor(QPalette::Inactive, QPalette::WindowText, backColor);

	
	// The palette of the parent label and each of the video and image widgets is set
	// to be the background color. Note that the MediaManager owns the parent QLabel, but 
	// the background color of it is set here. 
	
	if (qobject_cast<QLabel *>(parent_))
	{
		parent_->setPalette(palette);
		if (_mediaSwitcherFlag)
		_firstImageWgt->setPalette(palette);
		else
		_secondImageWgt->setPalette(palette);

		if (_mediaSwitcherFlag)	
		{
			for (int i = 0; i < _firstVideoWidget->children().count(); i++)
			{			
				QWidget * wgt = qobject_cast<QWidget*>(_firstVideoWidget->children()[i]);
				if (wgt)
				{
					wgt->setPalette(palette);
					wgt->setAutoFillBackground(true);
				}
			}
		}
		else	
		{
			for (int i = 0; i < _secondVideoWidget->children().count(); i++)
			{			
				QWidget * wgt = qobject_cast<QWidget*>(_secondVideoWidget->children()[i]);
				if (wgt)
				{
					wgt->setPalette(palette);
					wgt->setAutoFillBackground(true);
				}
			}
		}
	}
} 

void MediaPlayer::onFirstAboutToFinish(qint32 msec)
{
	Q_UNUSED(msec);
	if (_isLoopBack)
	{
		_firstMediaObject->pause();
		//if ((stopTime_) > 0)
		//	stopTime_-=_firstMediaObject->totalTime()+1;
		_firstMediaObject->seek(0);
		_waitForPlayed = true;
		_firstMediaObject->play();
	}
}

void MediaPlayer::onSecondAboutToFinish(qint32 msec)
{
	Q_UNUSED(msec);
	if (_isLoopBack)
	{
		_secondMediaObject->pause();
		//if ((stopTime_) > 0)
		//	stopTime_-=_secondMediaObject->totalTime()+1;
		_secondMediaObject->seek(0);
		_waitForPlayed = true;
		_secondMediaObject->play();
	}
}

void MediaPlayer::repeatCurrentMedia()
{
	if (!isImage_)
	{
		_currentMediaObject->pause();
		_currentMediaObject->seek(0);
		//timer_->start(10);
		_waitForPlayed = true;
		_currentMediaObject->play();
		if (_currentMediaObject->state() == Phonon::PlayingState)
		{
			_waitForPlayed = false;
			emit videoStarted();
		}
		_currentVideoWidget->show();
		isPlaying_ = true;
	}
	else
	{
		_currentImageWgt->show();
		startImageTime_ = QTime::currentTime();
		emit videoStarted();
		//timer_->start(10);

	}
	
}

void MediaPlayer::stopCurrentMedia()
{
	if (!isImage_)
	{
		_currentMediaObject->pause();
		_currentMediaObject->seek(0);
	}
}

}// namespace System