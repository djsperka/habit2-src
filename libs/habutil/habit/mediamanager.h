#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

#include "stimulusinfo.h"
#include "experimentsettings.h"
#include "mediaplayer.h"

#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtCore/QTimer>

namespace System {

class MediaManager : public QWidget
{
	Q_OBJECT

public:
	MediaManager(Habit::ExperimentSettings es, QWidget *parent = 0);
	~MediaManager();

	bool play();
	void loadNext(const Habit::StimulusSettings& ss, QColor backColor);
	void switchToNext();
	void stop();
	void clear();
	void repeat();
	void stopForRepeat();

protected slots:
	void onLeftMediaFinished();
	void onCenterMediaFinished();
	void onRightMediaFinished();
	void onControlMediaFinished();
	void onLeftMediaTimeFinished();
	void onCenterMediaTimeFinished();
	void onRightMediaTimeFinished();
	void onControlMediaTimeFinished();
	void onTimeout();
	void fullclear();
	void onVideoStarted();

signals:
	void playbackFinished();
	void cleared();
	void playingStarted();

private:
	int determineSleepTime(int stimulusType);

	int streamsCount_;
	bool leftFinished;
	bool centerFinished;
	bool rightFinished;
	bool controlFinished;

	bool videoStarted;

	Habit::ExperimentSettings esettings_;
	Habit::StimulusSettings _activessettings;
	Habit::StimulusSettings _backssettings;

	QWidget* screenWgt_;

	QLabel*	leftLabel_;
	QLabel*	centerLabel_;
	QLabel*	rightLabel_;
	QLabel*	controlLabel_;
	QColor	_backbackColor;
	QColor	_activebackColor;

	MediaPlayer* leftPlayer_;
	MediaPlayer* centerPlayer_;
	MediaPlayer* rightPlayer_;
	MediaPlayer* controlPlayer_;

	QTimer* timer_;

};

}
#endif // MEDIAMANAGER_H
