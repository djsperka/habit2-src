/*
 *  HMediaManagerUtil.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 9/18/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HMediaManagerUtil.h"
#ifdef USING_QT5
#include "H5VideoImagePlayer.h"
#include "H5AudioPlayer.h"
#else
#include "HVideoImagePlayer.h"
#include "HAudioPlayer.h"
#endif

void populatePlayers(const Habit::TrialsInfo& ti, const Habit::StimuliSettings& ss, Habit::IdStimulusSettingsPairList& idspStim, HPlayer* playerLeft, HPlayer* playerCenter, HPlayer* playerRight, HPlayer* playerControl);


HMediaManager* createMediaManager(const Habit::ExperimentSettings& es, QWidget* parent)
{
	Habit::IdStimulusSettingsPairList idsp1, idsp2, idsp3;
	return createMediaManager(es, parent, idsp1, idsp2, idsp3);
}


HMediaManager* createMediaManager(const Habit::ExperimentSettings& es, QWidget* parent, Habit::IdStimulusSettingsPairList& idspStimPreTest, Habit::IdStimulusSettingsPairList& idspStimHabituation, Habit::IdStimulusSettingsPairList& idspStimTest)
{
	Q_UNUSED(parent);
	HMediaManager* pmm = new HMediaManager();
	HPlayer *playerLeft = NULL;
	HPlayer *playerCenter = NULL;
	HPlayer *playerRight = NULL;
	HPlayer *playerControl = NULL;
	
	// Monitor settings
	Habit::MonitorSettings ms = es.getMonitorSettings();
	
	// AttentionGetter
	Habit::AttentionGetterSettings ags = es.getAttentionGetterSettings();
	
	// Trials info tell us how many trials in each phase
	Habit::DesignSettings ds = es.getDesignSettings();
	Habit::TrialsInfo tiPreTest = ds.getPretestTrialsInfo();
	Habit::TrialsInfo tiHabituation = ds.getHabituationTrialsInfo();
	Habit::TrialsInfo tiTest = ds.getTestTrialsInfo();
	
	// Now look at stimuli settings for each of the three phases. 
	Habit::StimuliSettings ssPreTest = es.getPreTestStimuliSettings();
	Habit::StimuliSettings ssHabituation = es.getHabituationStimuliSettings();
	Habit::StimuliSettings ssTest = es.getTestStimuliSettings();
	
	// Create players for each monitor that will be used. 
	// Non-NULL player pointers will indicate that a particular player was assigned to a monitor. 
	if ((ms.getLeftMonitorIndex() > -1)) 
	{
		playerLeft = new HVideoImagePlayer(ms.getLeftMonitorIndex());
		pmm->addPlayer(playerLeft, ms.getLeftMonitorIndex());
	}
	if (ms.getCenterMonitorIndex() > -1)
	{
		playerCenter = new HVideoImagePlayer(ms.getCenterMonitorIndex());
		pmm->addPlayer(playerCenter, ms.getCenterMonitorIndex());
	}
	if ((ms.getRightMonitorIndex() > -1)) 
	{
		playerRight = new HVideoImagePlayer(ms.getRightMonitorIndex());
		pmm->addPlayer(playerRight, ms.getRightMonitorIndex());
	}
	if ((ms.getControlMonitorIndex() > -1)) 
	{
		playerControl = new HAudioPlayer(ms.getControlMonitorIndex());
		pmm->addPlayer(playerControl, ms.getControlMonitorIndex());
	}
	
	// Attention getter? 
	if (ags.isAttentionGetterUsed())
	{
		Habit::StimulusSettings ssAG = ags.getAttentionGetterStimulus();
		if (playerLeft)
		{
			if (ssAG.isLeftEnabled())
			{
				Habit::StimulusInfo si = ssAG.getLeftStimulusInfo();
				playerLeft->addAG(si.getFileName(), si.getAudioBalance(), si.isLoopPlayBack());
			}
			else 
			{
				qWarning("Left monitor is configured, and attention getter is used, but no left monitor attention getter stimulus is configured.");
			}
		}
		if (playerCenter)
		{
			if (ssAG.isCenterEnabled())
			{
				Habit::StimulusInfo si = ssAG.getCenterStimulusInfo();
				playerCenter->addAG(si.getFileName(), si.getAudioBalance(), si.isLoopPlayBack());
			}
			else 
			{
				qWarning("Center monitor is configured, and attention getter is used, but no center monitor attention getter stimulus is configured.");
			}
		}
		if (playerRight)
		{
			if (ssAG.isRightEnabled())
			{
				Habit::StimulusInfo si = ssAG.getRightStimulusInfo();
				playerRight->addAG(si.getFileName(), si.getAudioBalance(), si.isLoopPlayBack());
			}
			else 
			{
				qWarning("Right monitor is configured, and attention getter is used, but no right monitor attention getter stimulus is configured.");
			}
		}
		if (playerControl)
		{
			if (ssAG.isIndependentSoundEnabled())
			{
				Habit::StimulusInfo si = ssAG.getIndependentSoundInfo();
				playerControl->addAG(si.getFileName(), si.getAudioBalance(), si.isLoopPlayBack());
			}
			else 
			{
				qWarning("Control monitor is configured, and attention getter is used, but no independent sound attention getter stimulus is configured.");
			}
		}
	}
	
	// The number of trials for a phase doesn't necessarily match the number of stimuli configured. Populate the player using the configured
	// stimuli only. For each, save the stimuli numbers as they are added to the players. 
	//
	// Habit does not require that stimuli be configured for each monitor that will be used. Whether its inadvertent or not, we must allow this 
	// to occur. One assumption we are making is that each of the players will have the same number of stimuli configured. Thus, if stimulus '10' is 
	// requested, each of the configured players should play that stimulus on each configured screen.
	//
	
	populatePlayers(tiPreTest, ssPreTest, idspStimPreTest, playerLeft, playerCenter, playerRight, playerControl);
	populatePlayers(tiHabituation, ssHabituation, idspStimHabituation, playerLeft, playerCenter, playerRight, playerControl);
	populatePlayers(tiTest, ssTest, idspStimTest, playerLeft, playerCenter, playerRight, playerControl);
	
	return pmm;
}


void populatePlayers(const Habit::TrialsInfo& ti, const Habit::StimuliSettings& ss, Habit::IdStimulusSettingsPairList& idspStim, HPlayer* playerLeft, HPlayer* playerCenter, HPlayer* playerRight, HPlayer* playerControl)
{
	int iStimNumber=0;
	int iTemp = 0;
	if (ti.getNumberOfTrials() > 0)
	{
		for (int i=0; i<ss.getStimuli().size(); i++)
		{
			iStimNumber = 0;
			Habit::StimulusSettings settings = ss.getStimuli()[i];
			if (playerLeft)
			{
				if (settings.isLeftEnabled())
				{
					Habit::StimulusInfo si = settings.getLeftStimulusInfo();
					iTemp = playerLeft->addStimulus(si.getFileName(), si.getAudioBalance(), si.isLoopPlayBack());
				}
				else 
				{
					iTemp = playerLeft->addStimulus();
				}
				if (iStimNumber==0) iStimNumber = iTemp;
			}
			
			if (playerCenter)
			{
				if (settings.isCenterEnabled())
				{
					Habit::StimulusInfo si = settings.getCenterStimulusInfo();
					iTemp = playerCenter->addStimulus(si.getFileName(), si.getAudioBalance(), si.isLoopPlayBack());
				}
				else 
				{
					iTemp = playerCenter->addStimulus();
				}
				if (iStimNumber==0) iStimNumber = iTemp;
			}
			
			if (playerRight)
			{
				if (settings.isRightEnabled())
				{
					Habit::StimulusInfo si = settings.getRightStimulusInfo();
					playerRight->addStimulus(si.getFileName(), si.getAudioBalance(), si.isLoopPlayBack());
				}
				else 
				{
					iTemp = playerRight->addStimulus();
				}
				if (iStimNumber==0) iStimNumber = iTemp;
			}
			
			if (playerControl)
			{
				if (settings.isIndependentSoundEnabled())
				{
					Habit::StimulusInfo si = settings.getIndependentSoundInfo();
					playerControl->addStimulus(si.getFileName(), si.getAudioBalance(), si.isLoopPlayBack());
				}
				else 
				{
					iTemp = playerControl->addStimulus();
				}
				if (iStimNumber==0) iStimNumber = iTemp;
			}
			idspStim.append(QPair<int, Habit::StimulusSettings>(iStimNumber, settings));
		}
	}
	return;
}
