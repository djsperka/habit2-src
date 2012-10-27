#include "testmisc.h"
#include "HTrialGenerator.h"
#include "HTrialLog.h"
#include "HPhaseLog.h"
#include "HPhaseCriteria.h"
#include <QTemporaryFile>
//#include "HOutputGenerator.h"

TestHabutil::TestHabutil()
: QObject()
{
}


void TestHabutil::testTrialLog()
{
	Habit::StimulusSettings settings;
	Habit::StimulusInfo leftSI;
	Habit::StimulusInfo centerSI;
	Habit::StimulusInfo rightSI;
	Habit::StimulusInfo soundSI;

	leftSI.setName("LeftName");
	leftSI.setFileName("LeftFileName");
	leftSI.setLoopPlayBack(true);
	leftSI.setAudioBalance(99);

	rightSI.setName("RightName");
	rightSI.setFileName("RightFileName");
	rightSI.setLoopPlayBack(true);
	rightSI.setAudioBalance(98);

	centerSI.setName("CenterName");
	centerSI.setFileName("CenterFileName");
	centerSI.setLoopPlayBack(false);
	centerSI.setAudioBalance(97);

	soundSI.setName("SoundName");
	soundSI.setFileName("SoundFileName");
	soundSI.setLoopPlayBack(true);
	soundSI.setAudioBalance(96);
	
	settings.setName("TestSettings");
	settings.setId(1);
	settings.setLeftEnabled(true);
	settings.setLeftStimulusInfo(leftSI);
	settings.setCenterEnabled(true);
	settings.setCenterStimulusInfo(centerSI);
	settings.setRightEnabled(true);
	settings.setRightStimulusInfo(rightSI);
	settings.setIndependentSoundEnabled(true);
	settings.setIndependentSoundInfo(soundSI);
	settings.setStimulusType(Habit::StimulusSettings::HABITUATION);

	HTrialLog tlog(settings);
	tlog.append(HLook(LookLeft, 100, 1100));
	tlog.append(HLook(LookLeft, 2200, 3200));
	tlog.append(HLook(LookLeft, 3300, 4300));
	tlog.append(HLook(LookLeft, 4400, 5400));
	tlog.setCompleted();	
	
	QVERIFY(tlog.isCompleted());
	QCOMPARE(tlog.totalLookingTime(), 4000);
	
	
	// Test output and input...
	QTemporaryFile file;
	
	HTrialLog tlog2(settings);
	
	// doesnt compile 
	// HTrialLog tlog2(Habit::StimulusSettings());
	
	// compiles fine
	// HTrialLog tlog2;
	
	if (file.open())
	{
		QDataStream stream(&file);
		stream << tlog;
		file.setAutoRemove(false);
		file.seek(0);
		stream >> tlog2;
		QVERIFY(tlog==tlog2);
		file.close();
	}
	else 
	{
		QFAIL("Cannot open temp file");
	}
	
}

void TestHabutil::createStimulusSettings(int i, Habit::StimulusSettings& settings)
{
	Habit::StimulusInfo leftSI;
	Habit::StimulusInfo centerSI;
	Habit::StimulusInfo rightSI;
	Habit::StimulusInfo soundSI;
	switch (i)
	{
		case 0:
		{
			leftSI.setName("LeftName0");
			leftSI.setFileName("LeftFileName0");
			leftSI.setLoopPlayBack(true);
			leftSI.setAudioBalance(1);
			
			rightSI.setName("RightName0");
			rightSI.setFileName("RightFileName0");
			rightSI.setLoopPlayBack(true);
			rightSI.setAudioBalance(1);
			
			centerSI.setName("CenterName0");
			centerSI.setFileName("CenterFileName0");
			centerSI.setLoopPlayBack(false);
			centerSI.setAudioBalance(2);
			
			soundSI.setName("SoundName0");
			soundSI.setFileName("SoundFileName0");
			soundSI.setLoopPlayBack(true);
			soundSI.setAudioBalance(3);
			
			settings.setName("TestSettings0");
			settings.setId(1);
			settings.setLeftEnabled(true);
			settings.setLeftStimulusInfo(leftSI);
			settings.setCenterEnabled(true);
			settings.setCenterStimulusInfo(centerSI);
			settings.setRightEnabled(true);
			settings.setRightStimulusInfo(rightSI);
			settings.setIndependentSoundEnabled(true);
			settings.setIndependentSoundInfo(soundSI);
			settings.setStimulusType(Habit::StimulusSettings::HABITUATION);
			
			break;
		}			
		case 1:
		{
			leftSI.setName("LeftName1");
			leftSI.setFileName("LeftFileName1");
			leftSI.setLoopPlayBack(true);
			leftSI.setAudioBalance(11);
			
			rightSI.setName("RightName1");
			rightSI.setFileName("RightFileName1");
			rightSI.setLoopPlayBack(true);
			rightSI.setAudioBalance(12);
			
			centerSI.setName("CenterName1");
			centerSI.setFileName("CenterFileName1");
			centerSI.setLoopPlayBack(false);
			centerSI.setAudioBalance(13);
			
			soundSI.setName("SoundName1");
			soundSI.setFileName("SoundFileName1");
			soundSI.setLoopPlayBack(true);
			soundSI.setAudioBalance(14);
			
			settings.setName("TestSettings1");
			settings.setId(1);
			settings.setLeftEnabled(true);
			settings.setLeftStimulusInfo(leftSI);
			settings.setCenterEnabled(true);
			settings.setCenterStimulusInfo(centerSI);
			settings.setRightEnabled(true);
			settings.setRightStimulusInfo(rightSI);
			settings.setIndependentSoundEnabled(true);
			settings.setIndependentSoundInfo(soundSI);
			settings.setStimulusType(Habit::StimulusSettings::HABITUATION);
			
			break;
		}			
		case 2:
		{
			leftSI.setName("LeftName2");
			leftSI.setFileName("LeftFileName2");
			leftSI.setLoopPlayBack(true);
			leftSI.setAudioBalance(21);
			
			rightSI.setName("RightName2");
			rightSI.setFileName("RightFileName2");
			rightSI.setLoopPlayBack(true);
			rightSI.setAudioBalance(22);
			
			centerSI.setName("CenterName2");
			centerSI.setFileName("CenterFileName2");
			centerSI.setLoopPlayBack(false);
			centerSI.setAudioBalance(23);
			
			soundSI.setName("SoundName2");
			soundSI.setFileName("SoundFileName2");
			soundSI.setLoopPlayBack(true);
			soundSI.setAudioBalance(24);
			
			settings.setName("TestSettings2");
			settings.setId(21);
			settings.setLeftEnabled(true);
			settings.setLeftStimulusInfo(leftSI);
			settings.setCenterEnabled(true);
			settings.setCenterStimulusInfo(centerSI);
			settings.setRightEnabled(true);
			settings.setRightStimulusInfo(rightSI);
			settings.setIndependentSoundEnabled(true);
			settings.setIndependentSoundInfo(soundSI);
			settings.setStimulusType(Habit::StimulusSettings::HABITUATION);
			
			break;
		}			
		case 3:
		default:
		{
			leftSI.setName("LeftName3");
			leftSI.setFileName("LeftFileName3");
			leftSI.setLoopPlayBack(true);
			leftSI.setAudioBalance(31);
			
			rightSI.setName("RightName3");
			rightSI.setFileName("RightFileName3");
			rightSI.setLoopPlayBack(true);
			rightSI.setAudioBalance(32);
			
			centerSI.setName("CenterName3");
			centerSI.setFileName("CenterFileName3");
			centerSI.setLoopPlayBack(false);
			centerSI.setAudioBalance(33);
			
			soundSI.setName("SoundName3");
			soundSI.setFileName("SoundFileName3");
			soundSI.setLoopPlayBack(true);
			soundSI.setAudioBalance(34);
			
			settings.setName("TestSettings3");
			settings.setId(31);
			settings.setLeftEnabled(true);
			settings.setLeftStimulusInfo(leftSI);
			settings.setCenterEnabled(true);
			settings.setCenterStimulusInfo(centerSI);
			settings.setRightEnabled(true);
			settings.setRightStimulusInfo(rightSI);
			settings.setIndependentSoundEnabled(true);
			settings.setIndependentSoundInfo(soundSI);
			settings.setStimulusType(Habit::StimulusSettings::HABITUATION);
			
			break;
		}			
	}
	return;
}

void TestHabutil::testPhaseLog()
{
	HPhaseLog l;
	Habit::StimulusSettings settings0;
	Habit::StimulusSettings settings1;
	Habit::StimulusSettings settings2;
	Habit::StimulusSettings settings3;
	
	connect(this, SIGNAL(trialStarted(Habit::StimulusSettings&)), &l, SLOT(trialStarted(Habit::StimulusSettings&)));
	connect(this, SIGNAL(trialCompleted()), &l, SLOT(trialCompleted()));
	connect(this, SIGNAL(look(HLook)), &l, SLOT(gotLook(HLook)));

	createStimulusSettings(0, settings0);
	emit trialStarted(settings0);
	emit look(HLook(LookLeft, 100, 1100));
	emit look(HLook(LookLeft, 2200, 3200));
	emit look(HLook(LookLeft, 3300, 4300));
	emit look(HLook(LookLeft, 4400, 5400));
	emit trialCompleted();	
	
	createStimulusSettings(1, settings1);
	emit trialStarted(settings1);
	emit look(HLook(LookLeft, 10100, 11100));
	emit look(HLook(LookLeft, 12200, 13200));
	emit look(HLook(LookLeft, 13300, 14300));
	emit look(HLook(LookLeft, 14400, 15400));
	emit trialCompleted();	

	createStimulusSettings(2, settings2);
	emit trialStarted(settings2);
	emit look(HLook(LookLeft, 20100, 21100));
	emit look(HLook(LookLeft, 22200, 23200));
	emit look(HLook(LookLeft, 23300, 24300));
	emit look(HLook(LookLeft, 24400, 25400));
	emit trialCompleted();	

	createStimulusSettings(3, settings3);
	emit trialStarted(settings3);
	emit look(HLook(LookLeft, 30100, 31100));
	emit look(HLook(LookLeft, 32200, 33200));
	emit look(HLook(LookLeft, 33300, 34300));
	emit look(HLook(LookLeft, 34400, 35400));
	emit trialCompleted();	
	
	disconnect(&l);

	
	// test counts
	QCOMPARE(l.nCompleted(), 4);
	QCOMPARE(l.totalLookingTime(), 16000);
	
	// Test output and input...
	QTemporaryFile file;
	if (file.open())
	{
		HPhaseLog ll;
		QDataStream stream(&file);
		stream << l;
		file.setAutoRemove(false);
		file.seek(0);
		stream >> ll;
		QVERIFY(l==ll);
		file.close();
	}
	else 
	{
		QFAIL("Cannot open temp file");
	}
	
}



void TestHabutil::testFixedNCriteria()
{
	HPhaseFixedNCriteria l(4);
	Habit::StimulusSettings settings0;
	Habit::StimulusSettings settings1;
	Habit::StimulusSettings settings2;
	Habit::StimulusSettings settings3;
	
	connect(this, SIGNAL(trialStarted(Habit::StimulusSettings&)), &l, SLOT(trialStarted(Habit::StimulusSettings&)));
	connect(this, SIGNAL(trialCompleted()), &l, SLOT(trialCompleted()));
	connect(this, SIGNAL(look(HLook)), &l, SLOT(gotLook(HLook)));
	
	createStimulusSettings(0, settings0);
	emit trialStarted(settings0);
	emit look(HLook(LookLeft, 100, 1100));
	emit look(HLook(LookLeft, 2200, 3200));
	emit look(HLook(LookLeft, 3300, 4300));
	emit look(HLook(LookLeft, 4400, 5400));
	emit trialCompleted();	

	QCOMPARE(l.nCompleted(), 1);
	QVERIFY(!l.isPhaseComplete());

	
	createStimulusSettings(1, settings1);
	emit trialStarted(settings1);
	emit look(HLook(LookLeft, 10100, 11100));
	emit look(HLook(LookLeft, 12200, 13200));
	emit look(HLook(LookLeft, 13300, 14300));
	emit look(HLook(LookLeft, 14400, 15400));
	emit trialCompleted();	

	QCOMPARE(l.nCompleted(), 2);
	QVERIFY(!l.isPhaseComplete());

	
	createStimulusSettings(2, settings2);
	emit trialStarted(settings2);
	emit look(HLook(LookLeft, 20100, 21100));
	emit look(HLook(LookLeft, 22200, 23200));
	emit look(HLook(LookLeft, 23300, 24300));
	emit look(HLook(LookLeft, 24400, 25400));
	emit trialCompleted();	

	QCOMPARE(l.nCompleted(), 3);
	QVERIFY(!l.isPhaseComplete());

	createStimulusSettings(3, settings3);
	emit trialStarted(settings3);
	emit look(HLook(LookLeft, 30100, 31100));
	emit look(HLook(LookLeft, 32200, 33200));
	emit look(HLook(LookLeft, 33300, 34300));
	emit look(HLook(LookLeft, 34400, 35400));
	emit trialCompleted();	
	
	disconnect(&l);
	
	
	// test counts
	QCOMPARE(l.nCompleted(), 4);
	QVERIFY(l.isPhaseComplete());
	
}




#if 0
void TestHabutil::testTrialLooks()
{
	QVERIFY(m_looksCompleted.isCompleted());
	QCOMPARE(m_looksNotCompleted.isCompleted(), false);
	QCOMPARE(m_looksCompleted.totalLookingTime(), 4000);
}

void TestHabutil::testFixedN()
{
	HPhaseFixedNCriteria c(3);
	
	connect(this, SIGNAL(look(HLook)), &c, SLOT(gotLook(HLook)));
	connect(this, SIGNAL(trialStarted()), &c, SLOT(trialStarted()));
	connect(this, SIGNAL(trialCompleted()), &c, SLOT(trialCompleted()));

	// Trial 1 1000ms
	emit trialStarted();
	emit look(HLook(LookLeft, 100, 1100));
	emit trialCompleted();

	QCOMPARE(c.isPhaseComplete(), false);
	
	// Trial 2 1000ms
	emit trialStarted();
	emit look(HLook(LookRight, 1200, 2200));
	emit trialCompleted();

	QCOMPARE(c.isPhaseComplete(), false);
	
	emit trialStarted();
	emit look(HLook(LookCenter, 2300, 3300));
	emit trialCompleted();

	QCOMPARE(c.isPhaseComplete(), true);

	disconnect(&c);
}



void TestHabutil::testTotalLookingTime()
{
	HPhaseTotalLookingTimeCriteria c(5000);
	
	connect(this, SIGNAL(look(HLook)), &c, SLOT(gotLook(HLook)));
	connect(this, SIGNAL(trialStarted()), &c, SLOT(trialStarted()));
	connect(this, SIGNAL(trialCompleted()), &c, SLOT(trialCompleted()));
	
	// trial 1 - 2000ms
	emit trialStarted();
	emit look(HLook(LookLeft, 100, 1100));
	emit look(HLook(LookLeft, 1200, 2200));
	emit trialCompleted();

	QCOMPARE(c.totalLookingTime(), 2000);
	QCOMPARE(c.isPhaseComplete(), false);

	// trial 2 - 2000ms
	emit trialStarted();
	emit look(HLook(LookRight, 2300, 3300));
	emit look(HLook(LookRight, 3400, 4400));
	emit trialCompleted();

	QCOMPARE(c.totalLookingTime(), 4000);
	QCOMPARE(c.isPhaseComplete(), false);
	
	// trial 3 - 500ms
	emit trialStarted();
	emit look(HLook(LookRight, 4500, 5000));
	emit trialCompleted();
	
	QCOMPARE(c.totalLookingTime(), 4500);
	QCOMPARE(c.isPhaseComplete(), false);

	// trial 4 - 1500ms
	emit trialStarted();
	emit look(HLook(LookRight, 5000, 6000));
	emit look(HLook(LookRight, 7000, 7500));
	emit trialCompleted();
	
	QCOMPARE(c.totalLookingTime(), 6000);
	QCOMPARE(c.isPhaseComplete(), true);
	
	disconnect(&c);
}

void TestHabutil::testHabituationFirstFixed()
{
	Habit::CriterionSettings cs(Habit::CriterionSettings::eFirstN, 50, 3, Habit::CriterionSettings::eFixedWindow);
	HPhaseHabituationCriteria c(cs);
	int itemp;
	int isum;
	
	// Basis - firstN  - FixedWindow 
	// first 3 trials form basis. Second fixed window fails, but third is OK.
	connect(this, SIGNAL(look(HLook)), &c, SLOT(gotLook(HLook)));
	connect(this, SIGNAL(trialStarted()), &c, SLOT(trialStarted()));
	connect(this, SIGNAL(trialCompleted()), &c, SLOT(trialCompleted()));
	
	// Trial 1 - 1000ms
	emit trialStarted();
	emit look(HLook(LookLeft, 100, 1100));
	emit trialCompleted();

	QVERIFY(!c.getBasisSum(isum, itemp));
	QCOMPARE(c.isPhaseComplete(), false);
	
	// trial 2 - 2000ms
	emit trialStarted();
	emit look(HLook(LookLeft, 1200, 3200));
	emit trialCompleted();

	QVERIFY(!c.getBasisSum(isum, itemp));
	QCOMPARE(c.isPhaseComplete(), false);
	
	// trial 3 - 3000ms
	emit trialStarted();
	emit look(HLook(LookRight, 2300, 3300));
	emit look(HLook(LookRight, 3400, 4400));
	emit look(HLook(LookRight, 4500, 5500));
	emit trialCompleted();

	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(isum, 6000);
	QCOMPARE(itemp, 0);
	QCOMPARE(c.isPhaseComplete(), false);
	
	// Now we have a basis - first 3 have 6000ms. The percent is 50, so we need a window
	// with less than 3000ms to complete. 
	// The next 3 trials will have 5000ms - no habituation.
	
	// Trial 4 - 1000ms
	emit trialStarted();
	emit look(HLook(LookLeft, 6100, 7100));
	emit trialCompleted();

	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(isum, 6000);
	QCOMPARE(itemp, 0);
	QCOMPARE(c.isPhaseComplete(), false);
	
	// trial 5 - 2000ms
	emit trialStarted();
	emit look(HLook(LookLeft, 7200, 9200));
	emit trialCompleted();

	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(isum, 6000);
	QCOMPARE(itemp, 0);
	QCOMPARE(c.isPhaseComplete(), false);
	
	// trial 6 - 2000ms
	emit trialStarted();
	emit look(HLook(LookRight, 10300, 11300));
	emit look(HLook(LookRight, 12400, 13400));
	emit trialCompleted();

	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(isum, 6000);
	QCOMPARE(itemp, 0);
	QCOMPARE(c.isPhaseComplete(), false);
	
	// now make the next 3 trials have just 2500ms - should be complete then.
	// Trial 7 - 1000ms
	emit trialStarted();
	emit look(HLook(LookLeft, 15000, 16000));
	emit trialCompleted();

	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(isum, 6000);
	QCOMPARE(itemp, 0);
	QCOMPARE(c.isPhaseComplete(), false);
	
	// trial 8 - 1000ms
	emit trialStarted();
	emit look(HLook(LookLeft, 20000, 21000));
	emit trialCompleted();

	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(isum, 6000);
	QCOMPARE(itemp, 0);
	QCOMPARE(c.isPhaseComplete(), false);
	
	// trial 9 - 500ms
	emit trialStarted();
	emit look(HLook(LookRight, 24000, 24500));
	emit trialCompleted();

	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(isum, 6000);
	QCOMPARE(itemp, 0);
	QCOMPARE(c.isPhaseComplete(), true);
	
	//Third 3 2500ms - should succeed.
	disconnect(&c);	
	
}

void TestHabutil::testHabituationLongestFixed()
{
	Habit::CriterionSettings cs(Habit::CriterionSettings::eLongestN, 50, 3, Habit::CriterionSettings::eFixedWindow);
	HPhaseHabituationCriteria c(cs);
	int itemp;
	int isum;
	
	// Basis - longestN  - FixedWindow 
	// Trial 1-3 2000ms
	// Trial 4-6 5000ms (longest)
	// Trial 7-9 3000ms 
	// Trial 10-12 2000ms (success)
	
	connect(this, SIGNAL(look(HLook)), &c, SLOT(gotLook(HLook)));
	connect(this, SIGNAL(trialStarted()), &c, SLOT(trialStarted()));
	connect(this, SIGNAL(trialCompleted()), &c, SLOT(trialCompleted()));
	
	// Trial 1 - 1000ms
	emit trialStarted();
	emit look(HLook(LookLeft, 100, 1100));
	emit trialCompleted();
	
	QVERIFY(!c.getBasisSum(isum, itemp));
	QCOMPARE(c.isPhaseComplete(), false);
	
	// trial 2 - 500ms
	emit trialStarted();
	emit look(HLook(LookLeft, 1200, 1700));
	emit trialCompleted();
	
	QVERIFY(!c.getBasisSum(isum, itemp));
	QCOMPARE(c.isPhaseComplete(), false);
	
	// trial 3 - 500ms
	emit trialStarted();
	emit look(HLook(LookRight, 2300, 2800));
	emit trialCompleted();
	
	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(isum, 2000);
	QCOMPARE(itemp, 0);
	QCOMPARE(c.isPhaseComplete(), false);
	
	// Trial 4 - 1000ms
	emit trialStarted();
	emit look(HLook(LookLeft, 6100, 7100));
	emit trialCompleted();
	
	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(isum, 2000);
	QCOMPARE(itemp, 0);
	QCOMPARE(c.isPhaseComplete(), false);
	
	// trial 5 - 2000ms
	emit trialStarted();
	emit look(HLook(LookLeft, 7200, 9200));
	emit trialCompleted();
	
	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(isum, 3500);
	QCOMPARE(itemp, 2);
	QCOMPARE(c.isPhaseComplete(), false);
	
	// trial 6 - 2000ms
	emit trialStarted();
	emit look(HLook(LookRight, 10300, 11300));
	emit look(HLook(LookRight, 12400, 13400));
	emit trialCompleted();

	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(isum, 5000);
	QCOMPARE(itemp, 3);
	QCOMPARE(c.isPhaseComplete(), false);
	
	// Trial 7 - 1000ms
	emit trialStarted();
	emit look(HLook(LookLeft, 15000, 16000));
	emit trialCompleted();
	
	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(isum, 5000);
	QCOMPARE(itemp, 3);
	QCOMPARE(c.isPhaseComplete(), false);
	
	// trial 8 - 1000ms
	emit trialStarted();
	emit look(HLook(LookLeft, 20000, 21000));
	emit trialCompleted();
	
	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(isum, 5000);
	QCOMPARE(itemp, 3);
	QCOMPARE(c.isPhaseComplete(), false);
	
	// trial 9 - 1000ms
	emit trialStarted();
	emit look(HLook(LookRight, 24000, 24500));
	emit look(HLook(LookRight, 25000, 25500));
	emit trialCompleted();
	
	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(isum, 5000);
	QCOMPARE(itemp, 3);
	QCOMPARE(c.isPhaseComplete(), false);
	
	// Trial 10 - 500ms
	emit trialStarted();
	emit look(HLook(LookLeft, 26000, 26500));
	emit trialCompleted();

	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(isum, 5000);
	QCOMPARE(itemp, 3);
	QCOMPARE(c.isPhaseComplete(), false);
	
	// Trial 11 - 500ms
	emit trialStarted();
	emit look(HLook(LookLeft, 27000, 27500));
	emit trialCompleted();

	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(isum, 5000);
	QCOMPARE(itemp, 3);
	QCOMPARE(c.isPhaseComplete(), false);
	
	// Trial 12 - 1000ms
	emit trialStarted();
	emit look(HLook(LookLeft, 28000, 29000));
	emit trialCompleted();

	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(isum, 5000);
	QCOMPARE(itemp, 3);
	QCOMPARE(c.isPhaseComplete(), true);
	
	disconnect(&c);
	
	
}

void TestHabutil::testHabituationFirstSliding()
{
	Habit::CriterionSettings cs(Habit::CriterionSettings::eFirstN, 50, 3, Habit::CriterionSettings::eSlidingWindow);
	HPhaseHabituationCriteria c(cs);
	int itemp;
	int isum;
	
	connect(this, SIGNAL(look(HLook)), &c, SLOT(gotLook(HLook)));
	connect(this, SIGNAL(trialStarted()), &c, SLOT(trialStarted()));
	connect(this, SIGNAL(trialCompleted()), &c, SLOT(trialCompleted()));
	
	// Trial 1 - 2000ms
	emit trialStarted();
	emit look(HLook(LookLeft, 100, 2100));
	emit trialCompleted();
	
	QVERIFY(!c.getBasisSum(isum, itemp));
	QVERIFY(!c.isPhaseComplete());
	
	// Trial 2 - 2000ms
	emit trialStarted();
	emit look(HLook(LookLeft, 2200, 2800));
	emit look(HLook(LookLeft, 3100, 3500));
	emit look(HLook(LookLeft, 3600, 4600));
	emit trialCompleted();
	
	QVERIFY(!c.getBasisSum(isum, itemp));
	QVERIFY(!c.isPhaseComplete());
	
	// Trial 3 - 2000ms. Basis sum = 6000, start trial 0
	emit trialStarted();
	emit look(HLook(LookLeft, 4000, 4800));
	emit look(HLook(LookLeft, 5000, 6200));
	emit trialCompleted();

	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(isum, 6000);
	QCOMPARE(itemp, 0);
	QVERIFY(!c.isPhaseComplete());
	
	// Trial 4 - 4000ms, Basis sum = 6000, start trial 0, sliding window 8000
	emit trialStarted();
	emit look(HLook(LookLeft, 7500, 9000));
	emit look(HLook(LookLeft, 9100, 10100));
	emit look(HLook(LookLeft, 11000, 12500));
	emit trialCompleted();
	
	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(isum, 6000);
	QCOMPARE(itemp, 0);
	QVERIFY(!c.isPhaseComplete());
	
	// Trial 5 - 1000ms. basis sum 6000,start trial 0. Sliding window 7000.
	emit trialStarted();
	emit look(HLook(LookLeft, 13000, 14000));
	emit trialCompleted();
	
	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(isum, 6000);
	QCOMPARE(itemp, 0);
	QVERIFY(!c.isPhaseComplete());
	
	// Trial 6 - 1500ms, basis sum 6000,start trial 0. Sliding window 6500
	emit trialStarted();
	emit look(HLook(LookLeft, 15000, 16500));
	emit trialCompleted();
	
	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(isum, 6000);
	QCOMPARE(itemp, 0);
	QVERIFY(!c.isPhaseComplete());
	
	// Trial 7 - 2000ms, basis sum 6000, start trial 0, sliding window 4500
	emit trialStarted();
	emit look(HLook(LookLeft, 16000, 17000));
	emit look(HLook(LookLeft, 18000, 19000));
	emit trialCompleted();
	
	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(isum, 6000);
	QCOMPARE(itemp, 0);
	QVERIFY(!c.isPhaseComplete());
	
	// Trial 8 - 1000ms, basis sum 6000, start trial 0, sliding window 4500
	emit trialStarted();
	emit look(HLook(LookLeft, 20000, 21000));
	emit trialCompleted();
	
	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(isum, 6000);
	QCOMPARE(itemp, 0);
	QVERIFY(!c.isPhaseComplete());
	
	// Trial 9 - 500ms, basis sum 6000, start trial 0, sliding window 3500
	emit trialStarted();
	emit look(HLook(LookLeft, 22000, 22500));
	emit trialCompleted();
	
	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(isum, 6000);
	QCOMPARE(itemp, 0);
	QVERIFY(!c.isPhaseComplete());

	// Trial 10 - 1000ms, basis sum 6000, start trial 0, sliding window 2500 (complete)
	emit trialStarted();
	emit look(HLook(LookLeft, 23000, 24000));
	emit trialCompleted();
	
	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(isum, 6000);
	QCOMPARE(itemp, 0);
	QVERIFY(c.isPhaseComplete());
	
	disconnect(&c);
}

void TestHabutil::testHabituationLongestSliding()
{
	Habit::CriterionSettings cs(Habit::CriterionSettings::eLongestN, 50, 3, Habit::CriterionSettings::eSlidingWindow);
	HPhaseHabituationCriteria c(cs);
	int itemp;
	int isum;
	
	connect(this, SIGNAL(look(HLook)), &c, SLOT(gotLook(HLook)));
	connect(this, SIGNAL(trialStarted()), &c, SLOT(trialStarted()));
	connect(this, SIGNAL(trialCompleted()), &c, SLOT(trialCompleted()));
	
	// Trial 1 - 2000ms
	emit trialStarted();
	emit look(HLook(LookLeft, 100, 2100));
	emit trialCompleted();
	
	QVERIFY(!c.getBasisSum(isum, itemp));
	QVERIFY(!c.isPhaseComplete());
	
	// Trial 2 - 2000ms
	emit trialStarted();
	emit look(HLook(LookLeft, 2200, 2800));
	emit look(HLook(LookLeft, 3100, 3500));
	emit look(HLook(LookLeft, 3600, 4600));
	emit trialCompleted();
	
	QVERIFY(!c.getBasisSum(isum, itemp));
	QVERIFY(!c.isPhaseComplete());
	
	// Trial 3 - 2000ms, basis sum 6000, start trial 0, sliding window 6000 (not complete)
	emit trialStarted();
	emit look(HLook(LookLeft, 4700, 6200));
	emit look(HLook(LookLeft, 6500, 7000));
	emit trialCompleted();
	
	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(itemp, 0);
	QCOMPARE(isum, 6000);
	QVERIFY(!c.isPhaseComplete());
	
	// Trial  - 4000ms, basis sum 8000, start trial 1, sliding window 8000 (not complete)
	emit trialStarted();
	emit look(HLook(LookLeft, 7500, 9000));
	emit look(HLook(LookLeft, 9100, 10100));
	emit look(HLook(LookLeft, 11000, 12500));
	emit trialCompleted();
	
	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(itemp, 1);
	QCOMPARE(isum, 8000);
	QVERIFY(!c.isPhaseComplete());
	
	// Trial 5 - 1500ms, basis sum 8000, start trial 1, sliding window 7500 (not complete)
	emit trialStarted();
	emit look(HLook(LookLeft, 13000, 14000));
	emit look(HLook(LookLeft, 14100, 14600));
	emit trialCompleted();
	
	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(itemp, 1);
	QCOMPARE(isum, 8000);
	QVERIFY(!c.isPhaseComplete());
	
	// Trial 6 - 500ms, basis sum 8000, start trial 1, sliding window 6000 (not complete)
	emit trialStarted();
	emit look(HLook(LookLeft, 15000, 15300));
	emit look(HLook(LookLeft, 15500, 15700));
	emit trialCompleted();
	
	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(itemp, 1);
	QCOMPARE(isum, 8000);
	QVERIFY(!c.isPhaseComplete());
	
	// Trial 7 - 2200ms, basis sum 8000, start trial 1, sliding window 4200 (not complete)
	emit trialStarted();
	emit look(HLook(LookLeft, 16000, 17000));
	emit look(HLook(LookLeft, 18000, 19200));
	emit trialCompleted();
	
	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(itemp, 1);
	QCOMPARE(isum, 8000);
	QVERIFY(!c.isPhaseComplete());
	
	// Trial 8 - 1000ms, basis sum 8000, start trial 1, sliding window 3700 (complete)
	emit trialStarted();
	emit look(HLook(LookLeft, 20000, 21000));
	emit trialCompleted();
	
	QVERIFY(c.getBasisSum(isum, itemp));
	QCOMPARE(itemp, 1);
	QCOMPARE(isum, 8000);
	QVERIFY(c.isPhaseComplete());
	
	disconnect(&c);
}
#endif

void TestHabutil::testTrialGenerator()
{
	HTrialGenerator ht1(4, false, false);
	for (unsigned int i=0; i<8; i++)
	{
		QCOMPARE(ht1.next(), i%4);
	}
	
	HTrialGenerator ht2(6, false, true);
	for (unsigned int i=0; i<12; i++)
	{
		QCOMPARE(ht2.next(), i%6);
	}

	HTrialGenerator ht3(5, true, true);
	QList<unsigned int> l3;	
	for (unsigned int i=0; i<5; i++)
		l3.append(0);
	
	// first round
	for (unsigned int i=0; i<5; i++)
	{
		unsigned int n = ht3.next();
		l3.replace(n, l3.at(n)+1);
	}
	
	// check that each element is 1
	for (unsigned int i=0; i<5; i++)
	{
		QCOMPARE(l3.at(i), (unsigned int)1);
	}

	// generate 10 more of each...
	for (unsigned int i=0; i<50; i++)
	{
		unsigned int n = ht3.next();
		l3.replace(n, l3.at(n)+1);
	}
	
	// check that each element is 11
	for (unsigned int i=0; i<5; i++)
	{
		QCOMPARE(l3.at(i), (unsigned int)11);
	}
	
}


QTEST_MAIN(TestHabutil)
//#include "testmisc.moc"
