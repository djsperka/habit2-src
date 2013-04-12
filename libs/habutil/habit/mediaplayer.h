
#ifndef SYSTEM_MEDIA_PLAYER_H
#define SYSTEM_MEDIA_PLAYER_H

#include "stimulusinfo.h"
#include "experimentsettings.h"
#include "imagewidget_original.h"

#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtCore/QTime>
#include <Phonon/MediaObject>

class QFile;
class QTimer;
class QString;

namespace Phonon {
	class MediaObject;
	class VideoWidget;
	class AudioOutput;
}

namespace System {

class MediaPlayer : public QWidget
{
	Q_OBJECT

public:
	MediaPlayer(QLabel* label=0, int monitorId = -1, QWidget* w = 0);
	~MediaPlayer();

private:
	int determineSleepTime(int stimulusType);
	void initializeLabelsWithMonitorSettings();
	void determineDisplaySize();
	bool isImage(const QString& fileName);
public:
	void dumpGeometry(const QString& label, QWidget *w);
	bool play(Habit::StimulusSettings& ss);
	//bool playMedia(const QString& file,  bool fullScreen = false, bool aspectRatio = true, QColor backColor = QColor());
	bool playMedia(QFile* f,  bool fullScreen = false);
	void setVolume(int vol);
	void stop();
	void playStop();
	void setStopTime(int stimTime);
	void setWindowMode(bool fullScreen, bool aspectRatio);
	void setBackColor(QColor backColor);
	void loadMediaToBuffer(const QString& file);
	void prepareToPlay();
	void playCurrentMedia();
	void repeatCurrentMedia();
	void stopCurrentMedia();
	void swapBuffers();
	bool IsLoopBack() const { return _isLoopBack; }
	void setIsLoopBack(bool val) { _isLoopBack = val; }

private slots:
	void onMediaFinished();
	void onTimeout();
	void onHasVideo(bool);
	void onStateChanged(Phonon::State, Phonon::State );
	void onCurrentMediaFinished();
	void onFirstMediaFinished();
	void onFirstHasVideo(  bool );
	void onFirstStateChanged(  Phonon::State,  Phonon::State );
	void onSecondMediaFinished();
	void onSecondHasVideo( bool );
	void onSecondStateChanged( Phonon::State, Phonon::State );
	void onFirstAboutToFinish(qint32);
	void onSecondAboutToFinish(qint32);
	

signals:
	void playbackFinished();
	void playTimeFinished();
	void videoStarted();
	


private:
	bool isPlaying_;
	bool isImage_;
	bool _isFullScreen;
	bool _isAspectRatio;
	QTime startImageTime_;
	bool	_isLoopBack;
	
	Habit::ExperimentSettings settings_;
	bool	_mediaSwitcherFlag;
	Phonon::MediaObject* _currentMediaObject;
	Phonon::MediaObject* _firstMediaObject;
	Phonon::MediaObject* _secondMediaObject;
	Phonon::VideoWidget* _currentVideoWidget;
	Phonon::VideoWidget* _firstVideoWidget;
	Phonon::VideoWidget* _secondVideoWidget;
	QLabel* parent_;
	ImageWidget* _currentImageWgt;
	ImageWidget* _firstImageWgt;
	ImageWidget* _secondImageWgt;
	Phonon::AudioOutput* _currentAudioOutput;
	Phonon::AudioOutput* _firstAudioOutput;
	Phonon::AudioOutput* _secondAudioOutput;
	int volume_;
	int stopTime_;
	Habit::StimulusInfo activeStimulusInfo_;
	int monitorId_;
	QString _previousFile;
	bool _waitForPlayed;
};

} // namespace System

#endif // SYSTEM_MEDIA_PLAYER_H