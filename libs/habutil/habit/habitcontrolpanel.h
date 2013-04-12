#ifndef HABITCONTROLPANEL_H
#define HABITCONTROLPANEL_H

#include "runsettings.h"
#include "stimulussettings.h"
#include "stimulisettings.h"
#include "experimentsettings.h"
#include "mediamanager.h"
#include "subjectsettings.h"
#include "OutputGenerator.h"

#include <QtGui/QDialog>
#include <QtCore/QTimer>
#include <QtCore/QTime>

class QPushButton;
class QLabel;
class QVBoxLayout;
class QString;

namespace GUILib {

class HabitControlPanel : public QDialog
{
	Q_OBJECT

public:
	HabitControlPanel(const Habit::SubjectSettings& ss, const Habit::RunSettings& runSettings, QWidget* w = 0);
	~HabitControlPanel();
	
	Habit::OutputGenerator getOutputGenerator() { return generator_; }
private:
	void createComponents();
	void makeConnections();
	void doLayout();
	void determineControlPanelSettings();
	void fillStimulus(const QString& order, const Habit::StimuliSettings& ss);
	void checkControlBarOptions(const Habit::ControlBarOptions& cbo);
	void playAttentionGetter();
	void playCurrentStimulus();
	void setStimulusLabelValues(Habit::StimulusSettings& stimulus);
	bool isMediaExists(Habit::StimulusSettings& stimulus);
	bool checkMedia();
	QString randomizeOrder(const QString& order, bool blocks = false, int increase = 0);
	bool processKeyboard(QTime time, int key);
	bool endCollectingData(QTime time);

private slots:
	void onStartTrials();
	void onNextTrial();
	void onStopTrials();
	void onPlayerFinish();
	void onTimeout();
	void onPlayerStart();
	void onNextTrialByButton();
	void onNextTrialByTime();

protected:
	//void keyPressEvent(QKeyEvent* e);
	//void keyReleaseEvent(QKeyEvent* e);
	void closeEvent (QCloseEvent * e); 
	bool eventFilter(QObject *target, QEvent *event);
	
private:
	Habit::RunSettings settings_;
	Habit::SubjectSettings subjectSettings_;
	QVector<Habit::StimulusSettings> runStimulus_;
	Habit::ExperimentSettings experimentSettings_;
	System::MediaManager* player_;
	int currentStimulusIndex_;
	bool attentionGetterPlayed_;
	bool isStarted_;

	QPushButton* startTrials_;
	QPushButton* nextTrial_;
	QPushButton* stopTrials_;

	QLabel* experimentStatus_;
	QLabel* lookStatus_;
	QLabel* attentionGetterStatus_;

	QLabel* experimentStatusValue_;
	QLabel* lookStatusValue_;
	QLabel* attentionGetterStatusValue_;

	QLabel* currentPhase_;
	QLabel* currentPhaseValue_;

	QLabel* currentStimulus_;
	QLabel* leftMonitorFile_;
	QLabel* centerMonitorFile_;
	QLabel* rightMonitorFile_;
	QLabel* soundFile_;
	
	QLabel* currentStimulusValue_;
	QLabel* leftMonitorFileValue_;
	QLabel* centerMonitorFileValue_;
	QLabel* rightMonitorFileValue_;
	QLabel* soundFileValue_;

	Habit::OutputGenerator generator_;
	QTime pressedTimeLeft_;
	QTime pressedTimeCenter_;
	QTime pressedTimeRight_;
	QTime lookAwayTime_;
	bool leftPressed_;
	bool centerPressed_;
	bool rightPressed_;
	bool nothingPressed_;
	bool leftLookNoted_;
	bool centerLookNoted_;
	bool rightLookNoted_;
	bool lookAwayNoted_;
	QTime trialstartTime_;
	QTimer*	timer_;
	QTimer* timerForFixedLength_;
	int previousKey_ ;
	QTime centerOnStartTime_;
	QTime centerOffStartTime_;
	QTime leftOnStartTime_;
	QTime leftOffStartTime_;
	QTime rightOnStartTime_;
	QTime rightOffStartTime_;
	QTime awayStartTime_;
	QTime centerCaptureTime_;
	QTime leftCaptureTime_;
	QTime rightCaptureTime_;
	bool centerCaptured_;
	bool leftCaptured_;
	bool rightCaptured_;
	bool awayCaptured_;
	bool centerFastCaptured_;
	bool leftFastCaptured_;
	bool rightFastCaptured_;
	//bool isEndOnLookAway_;
	bool isSubjectControlled;
	bool centerLookOnProcessing_;
	bool leftLookOnProcessing_;
	bool rightLookOnProcessing_;
	bool centerLookAwayProcessing_;
	bool leftLookAwayProcessing_;
	bool rightLookAwayProcessing_;
	bool anyLookCaptured_;
	bool _repeatTrial;
	bool _nextByBtn;
	int _currentMaxLen;
};

}
#endif // HABITCONTROLPANEL_H
