#include "habitcontrolpanel.h"
#include "maindao.h"
#include "experimentsettings.h"
#include "experimentresultsform.h"

#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QMessageBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QKeyEvent>
#include <QtCore/QDebug>
#include <QtGui/QApplication>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

namespace GUILib {

HabitControlPanel::HabitControlPanel(const Habit::SubjectSettings& ss, const Habit::RunSettings& runSettings, QWidget* w)
	: QDialog(w)
	, settings_(runSettings)
	, subjectSettings_(ss)
	, currentStimulusIndex_(0)
	, attentionGetterPlayed_(false)
	, isStarted_(false)
	, generator_()
	, pressedTimeLeft_()
	, pressedTimeCenter_ ()
	, pressedTimeRight_ ()
	, lookAwayTime_()
	, leftPressed_(false)
	, centerPressed_(false)
	, rightPressed_(false)
	, nothingPressed_(false)
	, leftLookNoted_(false)
	, centerLookNoted_(false)
	, rightLookNoted_(false)
	, trialstartTime_()
	, _nextByBtn(false)
{
	//Qt::WindowFlags wf = windowFlags();
	//wf |= Qt::WindowStaysOnTopHint;
	//setWindowFlags(wf);
	
	
	// The eventFilter() function (see below) receives all Qt-type events. It looks specifically at 
	// keypress events, and it determines when <enter>, <left>, <center> and <right> are pressed. In each
	// case it sets a bool var leftPressed_, centerPressed_ or rightPressed_, and that is picked up elsewhere. 
	// 
	// Note that the event filter is asynchronous, so those vars are set and un-set magically. 
	
	QCoreApplication::instance()->installEventFilter(this);
	//grabKeyboard();
	//You might also need to pass Qt::X11BypassWindowManagerHint for this to work.


	timer_ = new QTimer(this);
	connect(timer_, SIGNAL(timeout()), this, SLOT(onTimeout()));
	timer_->setSingleShot(true);
	timerForFixedLength_ = new QTimer(this);
	connect(timerForFixedLength_, SIGNAL(timeout()), this, SLOT(onNextTrialByTime()));
	timerForFixedLength_->setSingleShot(true);
	createComponents();
	makeConnections();
	doLayout();
	determineControlPanelSettings();
	stopTrials_->setEnabled(false);
	nextTrial_->setEnabled(false);
}

void HabitControlPanel::determineControlPanelSettings() {
	int expId = settings_.getExperimentId();
	Habit::MainDao dao;
	experimentSettings_ = dao.getExperimentSettingsById(expId);
	experimentSettings_.loadFromDB();

	QTime tm = QTime::currentTime();
	qsrand(tm.msec() + tm.second()*1000);
	fillStimulus(settings_.isPretestRandomized()?
				 randomizeOrder(settings_.getPretestOrder(), (bool)settings_.getPretestRandomizeMethod())
				 :settings_.getPretestOrder(), 
				 experimentSettings_.getPreTestStimuliSettings());
	fillStimulus(settings_.isHabituationRandomized()?
				 randomizeOrder(settings_.getHabituationOrder(), (bool)settings_.getHabituationRandomizeMethod())
				 :settings_.getHabituationOrder(), 
				 experimentSettings_.getHabituationStimuliSettings());
	fillStimulus(settings_.isTestRandomized()?
				 randomizeOrder(settings_.getTestOrder(), (bool)settings_.getTestRandomizeMethod())
				 :settings_.getTestOrder(), 
				 experimentSettings_.getTestStimuliSettings());
	checkControlBarOptions(experimentSettings_.getControlBarOptions());
	
}

QString HabitControlPanel::randomizeOrder(const QString& order, bool blocks, int increase)
{
	Q_UNUSED(increase);
	QStringList sList = order.split(" ");
	QStringList newList;
	int i = 0;
	while (sList.count() > 0)
	{
		if (!blocks)
		{
			int count = sList.count();
			int index = (qrand() + QTime::currentTime().msec() + i) % count;
			newList.append(sList.takeAt(index));
		}
		else
		{
			//int c = sList.count();
			int count = qRound(sList.count()*1.0/3 + 0.5);
			int index = (qrand() + QTime::currentTime().msec() + i) % count;
			index *= 3;
			for (int i = 0; i < 3; i++)
			{
				if (index < sList.count())
				{
					newList.append(sList.takeAt(index));
				}
			}
		}
		i++;
	}
	qDebug() << "randomize order:" << newList.join(" ");
	return newList.join(" ");
}

void HabitControlPanel::fillStimulus(const QString& order, const Habit::StimuliSettings& ss)
{
	int maxCount = 0;
	if (ss.getStimContext() == HStimContext::HabituationPhase)
		maxCount = experimentSettings_.getDesignSettings().getHabituationTrialsInfo().getNumberOfTrials();
	if (ss.getStimContext() == HStimContext::PreTestPhase)
		maxCount = experimentSettings_.getDesignSettings().getPretestTrialsInfo().getNumberOfTrials();
	if (ss.getStimContext() == HStimContext::TestPhase)
		maxCount = experimentSettings_.getDesignSettings().getTestTrialsInfo().getNumberOfTrials();
	int count = 0;
	while (count < maxCount)
	{
		QStringList orderList = order.split(" ");
		if (orderList.count() < 1)
			return;
		for(QStringList::iterator it = orderList.begin(); it != orderList.end(); ++it) 
		{
			QString numStr = *it;
			if(!numStr.isEmpty()) 
			{
				bool ok;
				int num = numStr.toInt(&ok);
				if(ok) {
					if(num > 0 && (num <= ss.getStimuli().size())) 
					{
						Habit::StimulusSettings stim = ss.getStimuli()[num - 1];
						stim.setStimulusType(type);
						runStimulus_.append(stim);
						count++;
						if (count >= maxCount)
							break;
					}
				}
			}
		}
		if (count == 0)
			return;
	}
}

void HabitControlPanel::checkControlBarOptions(const Habit::ControlBarOptions& cbo) {
	if(cbo.isControlBarUsed()) {
		if(!cbo.isCurrentExperimentDisplayed()) {
			currentPhase_->setVisible(false);
			currentPhaseValue_->setVisible(false);
		}
		if(!cbo.isCurrentStimulusDisplayed()) {
			currentStimulus_->setVisible(false);
			currentStimulusValue_->setVisible(false);
			leftMonitorFile_->setVisible(false);
			leftMonitorFileValue_->setVisible(false);
			centerMonitorFile_->setVisible(false);
			centerMonitorFileValue_->setVisible(false);
			rightMonitorFile_->setVisible(false);
			rightMonitorFileValue_->setVisible(false);
			soundFile_->setVisible(false);
			soundFileValue_->setVisible(false);
		}
	} else {
		experimentStatus_->setVisible(false);
		experimentStatusValue_->setVisible(false);
		lookStatus_->setVisible(false);
		lookStatusValue_->setVisible(false);
		attentionGetterStatus_->setVisible(false);
		attentionGetterStatusValue_->setVisible(false);
		currentPhase_->setVisible(false);
		currentPhaseValue_->setVisible(false);
		currentStimulus_->setVisible(false);
		currentStimulusValue_->setVisible(false);
		leftMonitorFile_->setVisible(false);
		leftMonitorFileValue_->setVisible(false);
		centerMonitorFile_->setVisible(false);
		centerMonitorFileValue_->setVisible(false);
		rightMonitorFile_->setVisible(false);
		rightMonitorFileValue_->setVisible(false);
		soundFile_->setVisible(false);
		soundFileValue_->setVisible(false);
	}
}

HabitControlPanel::~HabitControlPanel()
{
}

void HabitControlPanel::createComponents()
{
	experimentStatus_ = new QLabel(tr("Experiment Status:"));
	lookStatus_ = new QLabel(tr("Look Status:"));
	attentionGetterStatus_ = new QLabel(tr("A.G. Status"));
	currentPhase_ = new QLabel(tr("Current Phase:"));
	currentStimulus_ = new QLabel(tr("Current Stimulus:"));
	leftMonitorFile_ = new QLabel(tr("Left Monitor File:"));
	centerMonitorFile_ = new QLabel(tr("Center Monitor File:"));
	rightMonitorFile_ = new QLabel(tr("Right Monitor File:"));
	soundFile_ = new QLabel(tr("Sound File:"));

	currentPhaseValue_ = new QLabel(); currentPhaseValue_->setMaximumWidth(250);
	currentStimulusValue_ = new QLabel(); currentStimulusValue_->setMaximumWidth(250);
	leftMonitorFileValue_ = new QLabel(); leftMonitorFileValue_->setMaximumWidth(250);
	centerMonitorFileValue_ = new QLabel(); centerMonitorFileValue_->setMaximumWidth(250);
	rightMonitorFileValue_ = new QLabel(); rightMonitorFileValue_->setMaximumWidth(250);
	soundFileValue_ = new QLabel(); soundFileValue_->setMaximumWidth(250);
	experimentStatusValue_ = new QLabel(); experimentStatusValue_->setMaximumWidth(250);
	lookStatusValue_ = new QLabel(); lookStatusValue_->setMaximumWidth(250);
	attentionGetterStatusValue_ = new QLabel(); attentionGetterStatusValue_->setMaximumWidth(250);

	startTrials_ = new QPushButton(tr("Start Trials"));
	nextTrial_ = new QPushButton(tr("Next Trial"));
	stopTrials_ = new QPushButton(tr("Stop Trials"));
}

void HabitControlPanel::makeConnections() {
	connect(startTrials_, SIGNAL(clicked()), this, SLOT(onStartTrials()));
	connect(nextTrial_, SIGNAL(clicked()), this, SLOT(onNextTrialByButton()));
	connect(stopTrials_, SIGNAL(clicked()), this, SLOT(onStopTrials()));
}

void HabitControlPanel::doLayout()
{
	QHBoxLayout* buttonsLayout = new QHBoxLayout();
	buttonsLayout->addWidget(startTrials_);
	buttonsLayout->addWidget(nextTrial_);
	buttonsLayout->addWidget(stopTrials_);

	QGridLayout* gl = new QGridLayout();
	gl->addWidget(experimentStatus_, 0, 0);
	gl->addWidget(lookStatus_, 1, 0);
	gl->addWidget(attentionGetterStatus_, 2, 0);
	gl->addWidget(currentPhase_, 3, 0);
	gl->addWidget(currentStimulus_, 4, 0);
	gl->addWidget(leftMonitorFile_, 5, 0);
	gl->addWidget(centerMonitorFile_, 6, 0);
	gl->addWidget(rightMonitorFile_, 7, 0);
	gl->addWidget(soundFile_, 8, 0);

	gl->addWidget(experimentStatusValue_, 0, 1);
	gl->addWidget(lookStatusValue_, 1, 1);
	gl->addWidget(attentionGetterStatusValue_, 2, 1);
	gl->addWidget(currentPhaseValue_, 3, 1);
	gl->addWidget(currentStimulusValue_, 4, 1);
	gl->addWidget(leftMonitorFileValue_, 5, 1);
	gl->addWidget(centerMonitorFileValue_, 6, 1);
	gl->addWidget(rightMonitorFileValue_, 7, 1);
	gl->addWidget(soundFileValue_, 8, 1);

	QHBoxLayout* hb = new QHBoxLayout();
	hb->addLayout(gl);
	hb->addStretch();

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->addLayout(buttonsLayout);
	mainLayout->addLayout(hb);
	mainLayout->addStretch();
}

void HabitControlPanel::onStartTrials() {

	// currentStimulusIndex_ is the index of the stimulus to be played for a given trial.
	// runStimulus_ is a vector of StimulusSettings objects. Each StimulusSetting object 
	// contains the information for a single stimulus - what screens it is played on, whether
	// there is an independent sound stim, the volume level, the filename. 
	
	currentStimulusIndex_ = 0;

	
	// Check whether the files corresponding to all stimuli are present and found on the system.
	// If any files are not found, user will be asked if they want to proceed or punt.
	
	if (!checkMedia())
	{
		
		return;
	}

	if(currentStimulusIndex_ < runStimulus_.size()) {
		nextTrial_->setEnabled(false);
		startTrials_->setEnabled(false);
		stopTrials_->setEnabled(false);
		experimentStatusValue_->setText("Preparing Media Player");
		QApplication::processEvents();
		
		// Create the MediaManager object. This object manages the play of all stimuli, whether they are on one
		// screen or many, sound or not, etc. It will also play the attention getter as needed. 
		
		player_ = new System::MediaManager(experimentSettings_);
		connect(player_, SIGNAL(playbackFinished()), this, SLOT(onPlayerFinish()));
		connect(player_, SIGNAL(playingStarted()), this, SLOT(onPlayerStart()));
		
		// The handling of the attention getter is clumsy and confusing. In the parlance of the application, 
		// it is 'loaded' like any other stimulus. The same functions used for regular stimuli are used for 
		// the attention getter. To distinguish the different situations (i.e. when playing an AG and when 
		// playing a regular stim) a bool flag (attentionGetterPlayed_) is used....I think. This all becomes 
		// rather confusing, especially when trials are repeated. 
		//
		// Below, the media manager is directed to load the first attention getter or stimulus. Note that the 
		// attention getter has a different background color setting than the stimuli (which all share the same
		// background color setting). 
		
		Habit::AttentionGetterSettings ags = experimentSettings_.getAttentionGetterSettings();
		if (ags.isAttentionGetterUsed() /*&& !attentionGetterPlayed_*/)
		{
			Habit::StimulusSettings ss = ags.getAttentionGetterStimulus();
			player_->loadNext(ss, ags.getBackGroundColor());
		}
		else
		{
			player_->loadNext(runStimulus_[currentStimulusIndex_], experimentSettings_.getStimulusDisplayInfo().getBackGroundColor() );
		}

		// the output generator acts as a kind of running log for each trial. 
		
		generator_.setExperiment(settings_.getExperimentId());
		generator_.setRunSettings(settings_);
		generator_.setSubjectInformation(subjectSettings_);
	
		isStarted_ = true;
		Habit::StimulusSettings ss = runStimulus_[currentStimulusIndex_];
		generator_.setCurrentTrial(ss.getStimulusType());
		
		leftPressed_ = false;
		centerPressed_ = false;
		rightPressed_ = false;
		nothingPressed_ = false;
		leftLookNoted_ = false;
		centerLookNoted_ = false;
		rightLookNoted_ = false;
		qDebug() << "ON-START-TRIALS";
		
		
		// OK, now we call onNextTrial(). That's the same slot used when you press the "Next Trial" button. 
		
		onNextTrial();
		
	}
}

bool HabitControlPanel::checkMedia()
{
	QStringList approvedFiles;
	Habit::AttentionGetterSettings ags = experimentSettings_.getAttentionGetterSettings();
	Habit::StimulusSettings ss = ags.getAttentionGetterStimulus();
	{	
		if (ss.isLeftEnabled() && !approvedFiles.contains(ss.getLeftStimulusInfo().getFileName()) && !QFile::exists(ss.getLeftStimulusInfo().getFileName()))
		{
			QMessageBox box(QMessageBox::Warning, "Media missing", QString( "Media file %1 is missing. Do you want to proceed?").arg(ss.getLeftStimulusInfo().getFileName()), QMessageBox::Yes | QMessageBox::No);
			if (box.exec() == QMessageBox::No)
			return false;
			else
				approvedFiles.append(ss.getLeftStimulusInfo().getFileName());
		}
		if (ss.isRightEnabled() && !approvedFiles.contains(ss.getRightStimulusInfo().getFileName()) && !QFile::exists(ss.getRightStimulusInfo().getFileName()))
		{
			QMessageBox box(QMessageBox::Warning, "Media missing", QString("Media file %1 is missing. Do you want to proceed?").arg(ss.getRightStimulusInfo().getFileName()), QMessageBox::Yes | QMessageBox::No);
			if (box.exec() == QMessageBox::No)
			return false;
			else
				approvedFiles.append(ss.getRightStimulusInfo().getFileName());
		}
		if (ss.isCenterEnabled() && !approvedFiles.contains(ss.getCenterStimulusInfo().getFileName()) && !QFile::exists(ss.getCenterStimulusInfo().getFileName()))
		{
			QMessageBox box(QMessageBox::Warning, "Media missing", QString("Media file %1 is missing. Do you want to proceed?").arg(ss.getCenterStimulusInfo().getFileName()), QMessageBox::Yes | QMessageBox::No);
			if (box.exec() == QMessageBox::No)
			return false;
			else
				approvedFiles.append(ss.getCenterStimulusInfo().getFileName());
		}
		if (ss.isIndependentSoundEnabled() && !approvedFiles.contains(ss.getIndependentSoundInfo().getFileName()) && !QFile::exists(ss.getIndependentSoundInfo().getFileName()))
		{
			QMessageBox box(QMessageBox::Warning, "Media missing", QString("Media file %1 is missing. Do you want to proceed?").arg(ss.getIndependentSoundInfo().getFileName()), QMessageBox::Yes | QMessageBox::No);
			if (box.exec() == QMessageBox::No)
			return false;
			else
				approvedFiles.append(ss.getIndependentSoundInfo().getFileName());
		}
	}
	for (int i = 0; i < runStimulus_.count(); i++)
	{
		if (runStimulus_[i].isLeftEnabled() && !approvedFiles.contains(runStimulus_[i].getLeftStimulusInfo().getFileName()) && !QFile::exists(runStimulus_[i].getLeftStimulusInfo().getFileName()))
		{
			QMessageBox box(QMessageBox::Warning, "Media missing", QString( "Media file %1 is missing. Do you want to proceed?").arg(runStimulus_[i].getLeftStimulusInfo().getFileName()), QMessageBox::Yes | QMessageBox::No);
			if (box.exec() == QMessageBox::No)
			return false;
			else
				approvedFiles.append(runStimulus_[i].getLeftStimulusInfo().getFileName());
		}
		if (runStimulus_[i].isRightEnabled() && !approvedFiles.contains(runStimulus_[i].getRightStimulusInfo().getFileName()) && !QFile::exists(runStimulus_[i].getRightStimulusInfo().getFileName()))
		{
			QMessageBox box(QMessageBox::Warning, "Media missing", QString("Media file %1 is missing. Do you want to proceed?").arg(runStimulus_[i].getRightStimulusInfo().getFileName()), QMessageBox::Yes | QMessageBox::No);
			if (box.exec() == QMessageBox::No)
			return false;
			else
				approvedFiles.append(runStimulus_[i].getRightStimulusInfo().getFileName());
		}
		if (runStimulus_[i].isCenterEnabled() && !approvedFiles.contains(runStimulus_[i].getCenterStimulusInfo().getFileName()) && !QFile::exists(runStimulus_[i].getCenterStimulusInfo().getFileName()))
		{
			QMessageBox box(QMessageBox::Warning, "Media missing", QString("Media file %1 is missing. Do you want to proceed?").arg(runStimulus_[i].getCenterStimulusInfo().getFileName()), QMessageBox::Yes | QMessageBox::No);
			if (box.exec() == QMessageBox::No)
			return false;
			else
				approvedFiles.append(runStimulus_[i].getCenterStimulusInfo().getFileName());
		}
		if (runStimulus_[i].isIndependentSoundEnabled() && !approvedFiles.contains(runStimulus_[i].getIndependentSoundInfo().getFileName()) && !QFile::exists(runStimulus_[i].getIndependentSoundInfo().getFileName()))
		{
			QMessageBox box(QMessageBox::Warning, "Media missing", QString("Media file %1 is missing. Do you want to proceed?").arg(runStimulus_[i].getIndependentSoundInfo().getFileName()), QMessageBox::Yes | QMessageBox::No);
			if (box.exec() == QMessageBox::No)
			return false;
			else
				approvedFiles.append(runStimulus_[i].getIndependentSoundInfo().getFileName());
		}
	}
	return true;
}

void HabitControlPanel::onPlayerStart()
{
	qDebug() << "onPlayerStart()  ++++++++++++++ //////////// ++++++++++";
	qDebug() << "HabitControlPanel::onPlayerStart()";
	experimentStatusValue_->setText("Experiment Running");
	nextTrial_->setEnabled(true);
	startTrials_->setEnabled(false);
	stopTrials_->setEnabled(true);
	bool testB = experimentSettings_.getAttentionGetterSettings().isAttentionGetterUsed();
	
	// If AG has been played, or if no AG is used in this expt, 
	// drop a note in the generator/log indicating new trial (or repeat trial) has started, 
	// process any pending keypress (why?), 
	// start two timers: timer_ and timerForFixedLength_ (why?)
	// Also set _repeatTrial to false. The way this var is used is ... difficult to understand. 
	
	if (attentionGetterPlayed_ || !testB)
	{
		qDebug() << "Not attention getter played";
		if (_repeatTrial)
		generator_.addLogItem(Habit::TrialLogItem::NEW_TRIAL_REPEAT, 0);
		else
		generator_.addLogItem(Habit::TrialLogItem::NEW_TRIAL, 0);
		
		QTime now  = trialstartTime_ = QTime::currentTime();
		
		if (centerPressed_)
			processKeyboard(now, 2);
		else
			if (leftPressed_)
				processKeyboard(now, 1);
			else
				if (rightPressed_)
					processKeyboard(now ,3);
				else
					processKeyboard(now, 0);
		qDebug() << "timer Started";
		//if (!isSubjectControlled)
		//{
		//	int len = experimentSettings_.getDesignSettings().getHabituationTrialsInfo().getLength();
			timerForFixedLength_->start(_currentMaxLen * 100);
		//}
		timer_->start(10);
		_repeatTrial = false;
	}
	
}

void HabitControlPanel::onNextTrial() {
	qDebug() << "HabitControlPanel::onNextTrial() " << QTime::currentTime().toString("hh:mm:ss.zzz") ;
	QTime time = QTime::currentTime();
	timerForFixedLength_->stop();
	lookStatusValue_->setText("");
	experimentStatusValue_->setText("Waiting for New Media");
	nextTrial_->setDisabled(true);

	// For a repeated trial whose AG has been played....
	// The func returns from this block!
	if (_repeatTrial && attentionGetterPlayed_)
	{
		qDebug() << "REPEAT == TRUE; ATTENTION PLAYED == TRUE; ";

		// I'm guessing that this sets things to replay the last stimulus. 
		player_->stopForRepeat();
		player_->switchToNext();
		Habit::StimulusSettings ss = runStimulus_[currentStimulusIndex_-1];
		generator_.setStimulusSettings(ss);
		setStimulusLabelValues(ss);
		attentionGetterStatusValue_->setText("");
		QString phaseText;
		int st = ss.getStimulusType();
		switch(st) {
		case Habit::StimulusSettings::HABITUATION :
			phaseText = "Habituation";
			
			break;
		case Habit::StimulusSettings::PRETEST :
			phaseText = "Pretest";
		
			break;
		case Habit::StimulusSettings::TEST :
			phaseText = "Test";
			
			break;
		}
		QFontMetrics fm = currentPhaseValue_->fontMetrics();
		currentPhaseValue_->setText(fm.elidedText( QString("%1 - %2").arg(phaseText).arg(ss.getName()), Qt::ElideRight, currentPhaseValue_->maximumWidth()) );
		timerForFixedLength_->start(_currentMaxLen * 100);
		player_->play();
		return;
	}

	
	// It seems that there could be a repeated trial whose AG has not been played. If that is the case, then
	// the line below seems incorrect. I have no idea. djs
	
	_repeatTrial =false;
	//bool bAGUsed = experimentSettings_.getAttentionGetterSettings().isAttentionGetterUsed();

	if (attentionGetterPlayed_) 
	{
		qDebug() << "REPEAT == FALSE; ATTENTION PLAYED == TRUE";
		timer_->stop();

		player_->stop();
	}
	else
	{
		if (currentStimulusIndex_ > 0)
		{
			qDebug() << "REPEAT == FALSE; ATTENTION PLAYED == FALSE; currentStimulusIndex_ = " << currentStimulusIndex_;
			timer_->stop();
		

			// Here, it appears that a trial is repeated if NO looks were captured, and the user didn't hit the 
			// "Next Trial" button.
			//
			// If this is a "repeat" trial under those conditions, then decrement the stimulus index (must have been
			// incremented elsewhere). 
			//
			// 
			
			_repeatTrial = !anyLookCaptured_ && !_nextByBtn;
			if (_repeatTrial)
			{
				endCollectingData(time);
				currentStimulusIndex_--;
				generator_.addLogItem(Habit::TrialLogItem::REPEAT_TRIAL, 0);
			}
			else
			{
				player_->stop();
				bool continueTrial = endCollectingData(time);
				if (!continueTrial)
				{
					while ((currentStimulusIndex_ < runStimulus_.size()) && (runStimulus_[currentStimulusIndex_].getStimulusType() == Habit::StimulusSettings::HABITUATION))
					{
						currentStimulusIndex_++;
					}
				}
			}
		}
	}
	
	if(currentStimulusIndex_ < runStimulus_.size()) 
	{
		
		qDebug() << "REPEAT == FALSE; ATTENTION == TRUE; currentStimulusIndex_ <  " << currentStimulusIndex_;
		Habit::StimulusSettings ss = runStimulus_[currentStimulusIndex_];
		if(generator_.getCurrentTrialType() != ss.getStimulusType()) 
		{
			//generator_.setTrialDuration(trialstartTime_.msecsTo(QTime::currentTime()));
			//trialstartTime_ = QTime::currentTime();
		}
		generator_.setStimulusSettings(ss);
		
		Habit::AttentionGetterSettings ags = experimentSettings_.getAttentionGetterSettings();
		Habit::StimulusSettings stimulusSettings(ags.getAttentionGetterStimulus());
		if(ags.isAttentionGetterUsed() && isMediaExists(stimulusSettings) && !attentionGetterPlayed_ && !_repeatTrial) 
		{
			qDebug() << "BEFORE playAttentionGetter()";
			playAttentionGetter();
			attentionGetterPlayed_ = true;
			qDebug() << "AFTER playAttentionGetter()";
		} 
		else 
		{
			qDebug() << "attentionGetterPlayed_************ " << attentionGetterPlayed_ << "habitcontrolpanel.cpp - line 538";
			playCurrentStimulus();
			attentionGetterPlayed_ = false;
			++currentStimulusIndex_;
		}
	} 
	else 
	{
		timer_->stop();
		disconnect(player_, SIGNAL(playbackFinished()), this, SLOT(onPlayerFinish()));
		connect(player_, SIGNAL(cleared()), this, SLOT(accept()));
		player_->clear();
		generator_.summarize();
		//releaseKeyboard();
		//experimentStatusValue_->setText("Experiment Finished");
		//accept();
	}
}

bool HabitControlPanel::isMediaExists(Habit::StimulusSettings &stimulus)
{
	return ((!stimulus.getLeftStimulusInfo().getFileName().isEmpty()  ) && (stimulus.isLeftEnabled())) ||
		((!stimulus.getCenterStimulusInfo().getFileName().isEmpty()  ) && (stimulus.isCenterEnabled())) ||
		((!stimulus.getRightStimulusInfo().getFileName().isEmpty()  ) && (stimulus.isRightEnabled()));
}

void HabitControlPanel::onStopTrials() {
	qDebug() << "ON STOP TRIALS";
	timer_->stop();
	disconnect(player_, SIGNAL(playbackFinished()), this, SLOT(onPlayerFinish()));
	connect(player_, SIGNAL(cleared()), this, SLOT(accept()));
	player_->clear();
	//releaseKeyboard();
	generator_.summarize();
	//accept();
}

void HabitControlPanel::onPlayerFinish() {
	qDebug() << "ON-PLAYER-FINISH";
	_nextByBtn = false;
	qDebug() << "onPlayerFinish  attentionGetterPlayed_  ****" << attentionGetterPlayed_;
//	if (attentionGetterPlayed_)
		onNextTrial();
}

bool HabitControlPanel::endCollectingData(QTime time)
{
	qDebug() << "LOG GENERATOR ----------- ";
	if (leftCaptured_)
	{
		int lookTime = leftCaptureTime_.msecsTo(time);
		generator_.addLogItem(Habit::TrialLogItem::LEFT_LOOK, lookTime);
	}
	if (rightCaptured_)
	{
		int lookTime = rightCaptureTime_.msecsTo(time);
		generator_.addLogItem(Habit::TrialLogItem::RIGHT_LOOK, lookTime);
	}
	if (centerCaptured_)
	{
		int lookTime = centerCaptureTime_.msecsTo(time);
		generator_.addLogItem(Habit::TrialLogItem::CENTER_LOOK, lookTime);
	}
	if (awayCaptured_ )
	{
		int awayTime = awayStartTime_.msecsTo(time);
		generator_.addLogItem(Habit::TrialLogItem::AWAY_LOOK, awayTime);
	}
	if (!anyLookCaptured_)
	{
		int awayTime = trialstartTime_.msecsTo(time);
		generator_.addLogItem(Habit::TrialLogItem::NO_LOOK, awayTime);
	}
	leftCaptured_ = false;
	//leftPressed_ = false;
	rightCaptured_ = false;
	//rightPressed_ = false;
	//centerPressed_ = false;
	centerCaptured_ = false;
	awayCaptured_ = false;
	centerLookOnProcessing_ = false;
	leftLookOnProcessing_ = false;
	rightLookOnProcessing_ = false;
	centerLookAwayProcessing_ = false;
	leftLookAwayProcessing_ = false;
	rightLookAwayProcessing_ = false;
	anyLookCaptured_ = false;
	previousKey_ = 0;
	
	if (generator_.getCurrentTrialType() == Habit::StimulusSettings::HABITUATION)
	{
		return !generator_.isHabituated();
	}
	else
		return true;
}

void HabitControlPanel::setStimulusLabelValues(Habit::StimulusSettings& stimulus) {
	QFontMetrics fm = currentStimulusValue_->fontMetrics();
	currentStimulusValue_->setText(fm.elidedText(stimulus.getName(), Qt::ElideRight, currentStimulusValue_->maximumWidth()));
	fm = leftMonitorFileValue_->fontMetrics();
	leftMonitorFileValue_->setText(fm.elidedText(stimulus.isLeftEnabled() ? stimulus.getLeftStimulusInfo().getFileName() : "", Qt::ElideMiddle, leftMonitorFileValue_->maximumWidth()));
	fm = centerMonitorFileValue_->fontMetrics();
	centerMonitorFileValue_->setText(fm.elidedText(stimulus.isCenterEnabled() ? stimulus.getCenterStimulusInfo().getFileName() : "", Qt::ElideMiddle, leftMonitorFileValue_->maximumWidth()));
	fm = rightMonitorFileValue_->fontMetrics();
	rightMonitorFileValue_->setText(fm.elidedText(stimulus.isRightEnabled() ? stimulus.getRightStimulusInfo().getFileName() : "", Qt::ElideMiddle, leftMonitorFileValue_->maximumWidth()));
	fm = soundFileValue_->fontMetrics();
	soundFileValue_->setText(fm.elidedText(stimulus.isIndependentSoundEnabled() ? stimulus.getIndependentSoundInfo().getFileName() : "", Qt::ElideMiddle, leftMonitorFileValue_->maximumWidth()));
}

void HabitControlPanel::playAttentionGetter() {
	qDebug() << "HabitControlPanel::playAttentionGetter()";
	Habit::AttentionGetterSettings ags = experimentSettings_.getAttentionGetterSettings();
	attentionGetterStatusValue_->setText("AttentionGetter Playing");
	currentPhaseValue_->setText("");
	lookStatusValue_->setText("");
	Habit::StimulusSettings stimulusSettings(ags.getAttentionGetterStimulus());
	setStimulusLabelValues(stimulusSettings);
	//Habit::StimulusSettings ss = ags.getAttentionGetterStimulus();
	//ss.setStimulusType(-1);
	
	player_->switchToNext();
	player_->play();
	qDebug() << "attentionGetterPlayed_************ " << attentionGetterPlayed_;
	if (currentStimulusIndex_ < runStimulus_.count())
	{
		Habit::StimulusSettings ss = runStimulus_[currentStimulusIndex_];
		player_->loadNext(ss, experimentSettings_.getStimulusDisplayInfo().getBackGroundColor());
	}
	
//	attentionGetterPlayed_ = true;
	
	qDebug() << "END PLAYING ATTENTION GETTER ************";

	
//	qDebug() << "BEFORE OnNextTrial() called from PlayAttentionGetter()";
//	onNextTrial();
//	qDebug() << "AFTER OnNextTrial() called from PlayAttentionGetter()";
}

void HabitControlPanel::playCurrentStimulus() {
	qDebug() << "START PLAYING CURRENT STIMULUS ************";
	Q_ASSERT(currentStimulusIndex_ < runStimulus_.size());
	Habit::StimulusSettings ss = runStimulus_[currentStimulusIndex_];
	attentionGetterStatusValue_->setText("");
	QString phaseText;
	int st = ss.getStimulusType();
	switch(st) {
		case Habit::StimulusSettings::HABITUATION :
			phaseText = "Habituation";
			generator_.changeTrial(Habit::StimulusSettings::HABITUATION);
			isSubjectControlled = experimentSettings_.getDesignSettings().getHabituationTrialsInfo().getType() == Habit::TrialsInfo::eSubjectControlled;
			_currentMaxLen = experimentSettings_.getDesignSettings().getHabituationTrialsInfo().getLength();
			break;
		case Habit::StimulusSettings::PRETEST :
			phaseText = "Pretest";
			generator_.changeTrial(Habit::StimulusSettings::PRETEST);
			isSubjectControlled = experimentSettings_.getDesignSettings().getPretestTrialsInfo().getType() == Habit::TrialsInfo::eSubjectControlled;
			_currentMaxLen = experimentSettings_.getDesignSettings().getPretestTrialsInfo().getLength();
			break;
		case Habit::StimulusSettings::TEST :
			phaseText = "Test";
			generator_.changeTrial(Habit::StimulusSettings::TEST);
			isSubjectControlled = experimentSettings_.getDesignSettings().getTestTrialsInfo().getType() == Habit::TrialsInfo::eSubjectControlled;
			_currentMaxLen = experimentSettings_.getDesignSettings().getTestTrialsInfo().getLength();
			break;
	}
	QFontMetrics fm = currentPhaseValue_->fontMetrics();
	currentPhaseValue_->setText(fm.elidedText( QString("%1 - %2").arg(phaseText).arg(ss.getName()), Qt::ElideRight, currentPhaseValue_->maximumWidth()) );
	setStimulusLabelValues(ss);
	if (_repeatTrial)
	{
		//if (!isSubjectControlled)
		//{
			//int len = experimentSettings_.getDesignSettings().getHabituationTrialsInfo().getLength();
			//timerForFixedLength_->start(_currentMaxLen * 100);
		//}
		Habit::AttentionGetterSettings ags = experimentSettings_.getAttentionGetterSettings();
		if (ags.isAttentionGetterUsed() /*&& !attentionGetterPlayed_*/)
		{
			player_->stopForRepeat();
			player_->switchToNext();
			timerForFixedLength_->stop();
//			attentionGetterPlayed_ = true;
			qDebug() << "attentionGetterPlayed_************ " << attentionGetterPlayed_ << "habitcontrolpanel.cpp - line 715";
			Habit::AttentionGetterSettings ags = experimentSettings_.getAttentionGetterSettings();
			attentionGetterStatusValue_->setText("AttentionGetter Playing");
			currentPhaseValue_->setText("");
			lookStatusValue_->setText("");
			Habit::StimulusSettings stimulusSettings(ags.getAttentionGetterStimulus());
			setStimulusLabelValues(stimulusSettings);

			player_->play();
		}
		else
		{
			timerForFixedLength_->start(_currentMaxLen * 100);
			player_->repeat();
		}
	}
	else
	{
		player_->switchToNext();
		//if (!isSubjectControlled)
		//{
			//int len = experimentSettings_.getDesignSettings().getHabituationTrialsInfo().getLength();
			timerForFixedLength_->start(_currentMaxLen * 100);
		//}
		player_->play();
		Habit::AttentionGetterSettings ags = experimentSettings_.getAttentionGetterSettings();
		if (ags.isAttentionGetterUsed() /*&& !attentionGetterPlayed_*/)
		{
			Habit::StimulusSettings ss = ags.getAttentionGetterStimulus();
			ss.setStimulusType(-1);
			player_->loadNext(ss, ags.getBackGroundColor());
			attentionGetterPlayed_ = true;
		}
		else
		{
			if (currentStimulusIndex_ < runStimulus_.count()-1)
			{
				Habit::StimulusSettings ss = runStimulus_[currentStimulusIndex_+1];
				player_->loadNext(ss, experimentSettings_.getStimulusDisplayInfo().getBackGroundColor());
			}
		}
	}
	
	
	centerCaptured_ = false;
	leftCaptured_ = false;
	rightCaptured_ = false;
	centerFastCaptured_ = false;
	leftFastCaptured_ = false;
	rightFastCaptured_ = false;
	awayCaptured_ = false;
	centerLookOnProcessing_ = false;
	leftLookOnProcessing_ = false;
	rightLookOnProcessing_ = false;
	centerLookAwayProcessing_ = false;
	leftLookAwayProcessing_ = false;
	rightLookAwayProcessing_ = false;
	anyLookCaptured_ = false;
	previousKey_ = 0;
	
	//timer_->start(10);
	qDebug() << "END PLAYING CURRENT STIMULUS ************";
}

bool HabitControlPanel::eventFilter(QObject *target, QEvent *event)
{
//	QWidget * wdg = QApplication::activeWindow ();
//	bool hasFoc = wdg->isActiveWindow();
//	if(hasFoc)
//		int hf = 1;
//	QWidgetList wdgList = QApplication::allWidgets ();
	activateWindow();
//    int tip = event->type();
//	if(tip==122 || tip==99)
//		setFocus(Qt::ActiveWindowFocusReason);
	if (event->type() == QEvent::KeyPress) 
	{
        QKeyEvent *e = static_cast<QKeyEvent *>(event);
       
		if (e->isAutoRepeat())
		return true;
		QTime now = QTime::currentTime();
		
		if(e->key() == Qt::Key_6) {
			leftPressed_ = true;
			qDebug() << "leftPressed";
		} else
		if(e->key() == Qt::Key_5) {
			centerPressed_ = true;
			qDebug() << "centerPressed";
		} else
		if(e->key() == Qt::Key_4) {
			rightPressed_ = true;
			qDebug() << "rightPressed";
		}
		if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
		{
			if(attentionGetterPlayed_)
			{
				if (isStarted_)
				{
					if (nextTrial_->isEnabled())
						onNextTrialByButton();
				}
				else
				{
					onStartTrials();
				}
			}

		}
		if (e->key() == Qt::Key_Escape)
		{
			qDebug() << "ESC PRESSED";
			onStopTrials();
		}
        return true;
    }
	if (event->type() == QEvent::KeyRelease) 
	{
        QKeyEvent *e = static_cast<QKeyEvent *>(event);
       
		if (e->isAutoRepeat())
		return true;
		QTime now = QTime::currentTime();
		if(e->key() == Qt::Key_6) {
			leftPressed_ = false;
			qDebug() << "leftReleased";
		} else
		if(e->key() == Qt::Key_5 ) {
			centerPressed_ = false;
			qDebug() << "centerReleased";
		} else
		if(e->key() == Qt::Key_4 ) {
			rightPressed_ = false;
			qDebug() << "rightReleased";
		}
        return true;
    }
    return QDialog::eventFilter(target, event);
}

void HabitControlPanel::closeEvent(QCloseEvent * e)
{
	QTime time = QTime::currentTime();
	timer_->stop();
	if (isStarted_)
	{
		e->ignore();
		connect(player_, SIGNAL(cleared()), this, SLOT(accept()));
		player_->clear();
		endCollectingData(time);
	}
	//releaseKeyboard();
}

/*void HabitControlPanel::keyPressEvent(QKeyEvent* e) {
	if (e->isAutoRepeat())
		return;
	QTime now = QTime::currentTime();
	
	if(e->key() == Qt::Key_6) {
		leftPressed_ = true;
		qDebug() << "leftPressed";
	} else
	if(e->key() == Qt::Key_5) {
		centerPressed_ = true;
		qDebug() << "centerPressed";
	} else
	if(e->key() == Qt::Key_4) {
		rightPressed_ = true;
		qDebug() << "rightPressed";
	}
}

void HabitControlPanel::keyReleaseEvent(QKeyEvent* e) {
	if (e->isAutoRepeat())
		return;
	QTime now = QTime::currentTime();
	int msecs;
	if(e->key() == Qt::Key_6) {
		leftPressed_ = false;
		qDebug() << "leftReleased";
	} else
	if(e->key() == Qt::Key_5 ) {
		centerPressed_ = false;
		qDebug() << "centerReleased";
	} else
	if(e->key() == Qt::Key_4 ) {
		rightPressed_ = false;
		qDebug() << "rightReleased";
	}
}*/

void HabitControlPanel::onTimeout()
{
	qDebug() << "onTimeout()  ++++++++++++++ //////////// ++++++++++";
	QTime now = QTime::currentTime();
	bool cont = false;
	if (centerPressed_)
		cont = processKeyboard(now, 2);
	else
		if (leftPressed_)
			cont = processKeyboard(now, 1);
		else
			if (rightPressed_)
				cont = processKeyboard(now ,3);
			else
				cont = processKeyboard(now, 0);
	if (cont)
	timer_->start(10);
}

bool HabitControlPanel::processKeyboard(QTime time, int key)
{
	qDebug() << time.toString("hh:mm:ss.zzz") << previousKey_ << key;
	qDebug() << leftCaptured_ << centerCaptured_ << rightCaptured_ << awayCaptured_;
	qDebug() << leftLookAwayProcessing_ << centerLookAwayProcessing_ << rightLookAwayProcessing_;
	int minLookTime = experimentSettings_.getDesignSettings().getPretestTrialsInfo().getLookTimes() * 100;
	int maxLookAwayTime = experimentSettings_.getDesignSettings().getHabituationTrialsInfo().getLookTimes() * 100;
	int maxNoLookTime = experimentSettings_.getDesignSettings().getTestTrialsInfo().getLookTimes() * 100;
	//isSubjectControlled = experimentSettings_.getDesignSettings().getHabituationTrialsInfo().getType() == Habit::TrialsInfo::eSubjectControlled;
	
	if (key == 2)
	{
		centerFastCaptured_ = true;
		leftFastCaptured_ = false;
		rightFastCaptured_ = false;
		if (previousKey_ == key)
		{
			if (!centerCaptured_)
			{
				int msec = centerOnStartTime_.msecsTo(time);
				if (msec > minLookTime)
				{
					centerCaptured_ = true;
					if (!anyLookCaptured_)
					{
						int lookTime = trialstartTime_.msecsTo(time);
						if (lookTime > minLookTime)
						{
							lookTime -= minLookTime;
							generator_.addLogItem(Habit::TrialLogItem::AWAY_LOOK, lookTime);
							centerCaptureTime_ = time.addMSecs(-minLookTime);;
						}
					}
					else
					centerCaptureTime_ = time;
					
					anyLookCaptured_ = true;
					
					if (leftCaptured_)
					{
						int lookTime = leftCaptureTime_.msecsTo(time);
						generator_.addLogItem(Habit::TrialLogItem::LEFT_LOOK, lookTime);
						leftLookAwayProcessing_ = false;
						leftCaptured_ = false;	
					}
					if (rightCaptured_)
					{
						int lookTime = rightCaptureTime_.msecsTo(time);
						generator_.addLogItem(Habit::TrialLogItem::RIGHT_LOOK, lookTime);
						rightLookAwayProcessing_ = false;
						rightCaptured_ = false;	
					}
					if (awayCaptured_)
					{
						int awayTime = awayStartTime_.msecsTo(time);
						generator_.addLogItem(Habit::TrialLogItem::AWAY_LOOK, awayTime);
						awayCaptured_ = false;	
					}
				} // msec > minLookTime
				else
				{

					if (leftCaptured_ && leftLookAwayProcessing_)
					{
						int lookTime = leftCaptureTime_.msecsTo(time);
						int awayTime = leftOffStartTime_.msecsTo(time);
						if (awayTime > maxLookAwayTime)
						{
							generator_.addLogItem(Habit::TrialLogItem::LEFT_LOOK, lookTime);
							leftLookAwayProcessing_ = false;
							leftCaptured_ = false;	
							awayCaptured_ = true;
							awayStartTime_ = time;
							if (isSubjectControlled)
							{
								onPlayerFinish();
								return false;
							}

						}
					}
					if (rightCaptured_ && rightLookAwayProcessing_)
					{
						int lookTime = rightCaptureTime_.msecsTo(time);
						int awayTime = rightOffStartTime_.msecsTo(time);
						if (awayTime > maxLookAwayTime)
						{
							generator_.addLogItem(Habit::TrialLogItem::RIGHT_LOOK, lookTime);
							rightLookAwayProcessing_ = false;
							rightCaptured_ = false;	
							awayCaptured_ = true;
							if (isSubjectControlled)
							{
								onPlayerFinish();
								return false;
							}
							awayStartTime_ = time;
						}
					}
				} // msce < minLookTime
			} // !centerCaptured
			else
			{
				
			}
		} // previouskey == key
		else
		{
			if (previousKey_ == 1)
			{
				if (leftCaptured_)
				{
					leftLookAwayProcessing_ = true;
					leftOffStartTime_ = time;
				} // leftCaptured
				else
				{
					leftLookOnProcessing_ = false;
				}

			} // previouskey == 1 == left
			if (previousKey_ == 3)
			{
				if (rightCaptured_)
				{
					rightLookAwayProcessing_ = true;
					rightOffStartTime_ = time;
				} // rightCaptured
				else
				{
					rightLookOnProcessing_ = false;
				}

			} // previouskey == 3 == right
			centerLookOnProcessing_ = true;
			centerOnStartTime_ = time;
		}
	}

	if (key == 1)
	{
		leftFastCaptured_ = true;
		centerFastCaptured_ = false;
		rightFastCaptured_ = false;
		if (previousKey_ == key)
		{
			if (!leftCaptured_)
			{
				int msec = leftOnStartTime_.msecsTo(time);
				if (msec > minLookTime)
				{
					leftCaptured_ = true;
					if (!anyLookCaptured_)
					{
						int lookTime = trialstartTime_.msecsTo(time);
						if (lookTime > minLookTime)
						{
							lookTime -= minLookTime;
							generator_.addLogItem(Habit::TrialLogItem::AWAY_LOOK, lookTime);
							leftCaptureTime_ = time.addMSecs(-minLookTime);;
						}
					}
					else
						leftCaptureTime_ = time;
					anyLookCaptured_ = true;
					
					if (centerCaptured_)
					{
						int lookTime = centerCaptureTime_.msecsTo(time);
						generator_.addLogItem(Habit::TrialLogItem::CENTER_LOOK, lookTime);
						centerLookAwayProcessing_ = false;
						centerCaptured_ = false;	
					}
					if (rightCaptured_)
					{
						int lookTime = rightCaptureTime_.msecsTo(time);
						generator_.addLogItem(Habit::TrialLogItem::RIGHT_LOOK, lookTime);
						rightLookAwayProcessing_ = false;
						rightCaptured_ = false;	
					}
					if (awayCaptured_)
					{
						int awayTime = awayStartTime_.msecsTo(time);
						generator_.addLogItem(Habit::TrialLogItem::AWAY_LOOK, awayTime);
						awayCaptured_ = false;	
					}
				} // msec > minLookTime
				else
				{

					if (centerCaptured_ && centerLookAwayProcessing_)
					{
						int lookTime = centerCaptureTime_.msecsTo(time);
						int awayTime = centerOffStartTime_.msecsTo(time);
						if (awayTime > maxLookAwayTime)
						{
							generator_.addLogItem(Habit::TrialLogItem::CENTER_LOOK, lookTime);
							centerLookAwayProcessing_ = false;
							centerCaptured_ = false;	
							awayCaptured_ = true;
							if (isSubjectControlled)
							{
								onPlayerFinish();
								return false;
							}
							awayStartTime_ = time;
						}
					}
					if (rightCaptured_ && rightLookAwayProcessing_)
					{
						int lookTime = rightCaptureTime_.msecsTo(time);
						int awayTime = rightOffStartTime_.msecsTo(time);
						if (awayTime > maxLookAwayTime)
						{
							generator_.addLogItem(Habit::TrialLogItem::RIGHT_LOOK, lookTime);
							rightLookAwayProcessing_ = false;
							rightCaptured_ = false;	
							awayCaptured_ = true;
							if (isSubjectControlled)
							{
								onPlayerFinish();
								return false;
							}
							awayStartTime_ = time;
						}
					}
				} // msce < minLookTime
			} // !centerCaptured
			else
			{
				
			}
		} // previouskey == key
		else
		{
			if (previousKey_ == 2)
			{
				if (centerCaptured_)
				{
					centerLookAwayProcessing_ = true;
					centerOffStartTime_ = time;
				} // centerCaptured
				else
				{
					centerLookOnProcessing_ = false;
				}

			} // previouskey == 2 == center
			if (previousKey_ == 3)
			{
				if (rightCaptured_)
				{
					rightLookAwayProcessing_ = true;
					rightOffStartTime_ = time;
				} // rightCaptured
				else
				{
					rightLookOnProcessing_ = false;
				}

			} // previouskey == 3 == right
			leftLookOnProcessing_ = true;
			leftOnStartTime_ = time;
		}
	}

	if (key == 3)
	{
		rightFastCaptured_ = true;
		centerFastCaptured_ = false;
		leftFastCaptured_ = false;
		if (previousKey_ == key)
		{
			if (!rightCaptured_)
			{
				int msec = rightOnStartTime_.msecsTo(time);
				if (msec > minLookTime)
				{
					rightCaptured_ = true;
					if (!anyLookCaptured_)
					{
						int lookTime = trialstartTime_.msecsTo(time);
						if (lookTime > minLookTime)
						{
							lookTime -= minLookTime;
							generator_.addLogItem(Habit::TrialLogItem::AWAY_LOOK, lookTime);
							rightCaptureTime_ = time.addMSecs(-minLookTime);;
						}
					}
					else
						rightCaptureTime_ = time;
					anyLookCaptured_ = true;
				
					if (leftCaptured_)
					{
						int lookTime = leftCaptureTime_.msecsTo(time);
						generator_.addLogItem(Habit::TrialLogItem::LEFT_LOOK, lookTime);
						leftLookAwayProcessing_ = false;
						leftCaptured_ = false;	
					}
					if (centerCaptured_)
					{
						int lookTime = centerCaptureTime_.msecsTo(time);
						generator_.addLogItem(Habit::TrialLogItem::CENTER_LOOK, lookTime);
						centerLookAwayProcessing_ = false;
						centerCaptured_ = false;	
					}
					if (awayCaptured_)
					{
						int awayTime = awayStartTime_.msecsTo(time);
						generator_.addLogItem(Habit::TrialLogItem::AWAY_LOOK, awayTime);
						awayCaptured_ = false;	
					}
				} // msec > minLookTime
				else
				{

					if (leftCaptured_ && leftLookAwayProcessing_)
					{
						int lookTime = leftCaptureTime_.msecsTo(time);
						int awayTime = leftOffStartTime_.msecsTo(time);
						if (awayTime > maxLookAwayTime)
						{
							generator_.addLogItem(Habit::TrialLogItem::LEFT_LOOK, lookTime);
							leftLookAwayProcessing_ = false;
							leftCaptured_ = false;	
							awayCaptured_ = true;
							if (isSubjectControlled)
							{
								onPlayerFinish();
								return false;
							}
							awayStartTime_ = time;
						}
					}
					if (centerCaptured_ && centerLookAwayProcessing_)
					{
						int lookTime = centerCaptureTime_.msecsTo(time);
						int awayTime = centerOffStartTime_.msecsTo(time);
						if (awayTime > maxLookAwayTime)
						{
							generator_.addLogItem(Habit::TrialLogItem::CENTER_LOOK, lookTime);
							centerLookAwayProcessing_ = false;
							centerCaptured_ = false;	
							awayCaptured_ = true;
							if (isSubjectControlled)
							{
								onPlayerFinish();
								return false;
							}
							awayStartTime_ = time;
						}
					}
				} // msce < minLookTime
			} // !centerCaptured
			else
			{
				
			}
		} // previouskey == key
		else
		{
			if (previousKey_ == 1)
			{
				if (leftCaptured_)
				{
					leftLookAwayProcessing_ = true;
					leftOffStartTime_ = time;
				} // leftCaptured
				else
				{
					leftLookOnProcessing_ = false;
				}

			} // previouskey == 1 == left
			if (previousKey_ == 2)
			{
				if (centerCaptured_)
				{
					centerLookAwayProcessing_ = true;
					centerOffStartTime_ = time;
				} // rightCaptured
				else
				{
					centerLookOnProcessing_ = false;
				}

			} // previouskey == 3 == right
			rightLookOnProcessing_ = true;
			rightOnStartTime_ = time;
		}
	}

	if (key == 0) // If no key pressed
	{
		rightFastCaptured_ = false;
		centerFastCaptured_ = false;
		leftFastCaptured_ = false;
		if (previousKey_ == key)
		{
			if (anyLookCaptured_)
			if (!awayCaptured_) // if look away hasn't been captured yet
			{
				int msec = awayStartTime_.msecsTo(time);
				if (msec > maxLookAwayTime)
				{
					awayCaptured_ = true;
					if (isSubjectControlled)
					{
						onPlayerFinish();
						return false;
					}
					awayStartTime_ = time;
					if (leftCaptured_)
					{
						int lookTime = leftCaptureTime_.msecsTo(time);
						generator_.addLogItem(Habit::TrialLogItem::LEFT_LOOK, lookTime);
						leftLookAwayProcessing_ = false;
						leftCaptured_ = false;	
					}
					if (rightCaptured_)
					{
						int lookTime = rightCaptureTime_.msecsTo(time);
						generator_.addLogItem(Habit::TrialLogItem::RIGHT_LOOK, lookTime);
						rightLookAwayProcessing_ = false;
						rightCaptured_ = false;	
					}
					if (centerCaptured_)
					{
						int lookTime = centerCaptureTime_.msecsTo(time);
						generator_.addLogItem(Habit::TrialLogItem::CENTER_LOOK, lookTime);
						centerLookAwayProcessing_ = false;
						centerCaptured_ = false;	
					}
				} // msec > maxLookAwayTime
			} // !awayCaptured
		} // previouskey == key == 0
		else
		{
			if (previousKey_ == 1)
			{
				if (leftCaptured_)
				{
					leftLookAwayProcessing_ = true;
					leftOffStartTime_ = time;
					awayStartTime_ = time;
				} // leftCaptured
				else
				{
					leftLookOnProcessing_ = false;
				}

			} // previouskey == 1 == left
			if (previousKey_ == 2)
			{
				if (centerCaptured_)
				{
					centerLookAwayProcessing_ = true;
					centerOffStartTime_ = time;
					awayStartTime_ = time;
				} // centerCaptured
				else
				{
					centerLookOnProcessing_ = false;
				}

			} // previouskey == 1 == left
			if (previousKey_ == 3)
			{
				if (rightCaptured_)
				{
					rightLookAwayProcessing_ = true;
					rightOffStartTime_ = time;
					awayStartTime_ = time;
				} // rightCaptured
				else
				{
					rightLookOnProcessing_ = false;
				}

			} // previouskey == 3 == right
		}
	} // key == 0

	previousKey_ = key;
	if (!anyLookCaptured_)
	{
		int nolookTime = trialstartTime_.msecsTo(time);
		if (nolookTime > maxNoLookTime)
		{
			_nextByBtn = false;
			onPlayerFinish();
			return false;
		}
	}
	if ((leftCaptured_) || (leftFastCaptured_))
	{
		lookStatusValue_->setText("Look Left");
	}
	else
	if ((centerCaptured_) || (centerFastCaptured_))
	{
		lookStatusValue_->setText("Look Center");
	}
	else
	if ((rightCaptured_) || (rightFastCaptured_))
	{
		lookStatusValue_->setText("Look Right");
	}
	else
	if (awayCaptured_)
	{
		lookStatusValue_->setText("Look Away");
	}
	else
	{
		lookStatusValue_->setText("No Look");
	}
	return true;

}

void HabitControlPanel::onNextTrialByButton()
{
	qDebug() << "ON-NEXT-TRIAL-BY-BUTTON";
	_nextByBtn = true;
	onNextTrial();
}

void HabitControlPanel::onNextTrialByTime()
{
	qDebug() << "ON-NEXT-TRIAL-BY-TIME";
	_nextByBtn = true;
	onNextTrial();
}

}