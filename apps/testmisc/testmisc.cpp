#include "testmisc.h"
#include "HEventLog.h"
#include "HEvents.h"
#include "HTrialGenerator.h"
#include "HPhaseLog.h"
#include "HPhaseCriteria.h"
#include <QTemporaryFile>

using namespace habit2;

TestHabutil::TestHabutil()
: QObject()
{
}

void TestHabutil::testEventLog()
{
	HEventLog& elog = HEventLog::instance();
	HPhaseLog plog;
	HPhaseFixedNCriteria fixedNCriteria(4);
	HPhaseTotalLookingTimeCriteria totalLookingTimeCriteria(10000);
	Habit::CriterionSettings csFirstNFixed(Habit::CriterionSettings::eFirstN, 50, 3, Habit::CriterionSettings::eFixedWindow);
	HPhaseHabituationCriteria criteriaFirstNFixed(csFirstNFixed);
	Habit::CriterionSettings csLongestNFixed(Habit::CriterionSettings::eLongestN, 50, 3, Habit::CriterionSettings::eFixedWindow);
	HPhaseHabituationCriteria criteriaLongestNFixed(csLongestNFixed);
	Habit::CriterionSettings csFirstNSliding(Habit::CriterionSettings::eFirstN, 50, 3, Habit::CriterionSettings::eSlidingWindow);
	HPhaseHabituationCriteria criteriaFirstNSliding(csFirstNSliding);
	Habit::CriterionSettings csLongestNSliding(Habit::CriterionSettings::eLongestN, 50, 3, Habit::CriterionSettings::eSlidingWindow);
	HPhaseHabituationCriteria criteriaLongestNSliding(csLongestNSliding);

	
	// test empty event log
	plog = elog.getPhaseLog(QString("PreTest"));
	QVERIFY(plog.size()==0);
	QCOMPARE(plog.totalLookingTime(), 0);
	QVERIFY(!fixedNCriteria.isPhaseComplete(plog));
	QVERIFY(!totalLookingTimeCriteria.isPhaseComplete(plog));

	plog = elog.getPhaseLog(QString(""));
	QVERIFY(plog.size()==0);
	QCOMPARE(plog.totalLookingTime(), 0);
	QVERIFY(!fixedNCriteria.isPhaseComplete(plog));
	QVERIFY(!totalLookingTimeCriteria.isPhaseComplete(plog));
	
	// Now add some events
	elog.append(new HPhaseStartEvent((char *)"PreTest", 1));
	elog.append(new HStimRequestEvent(1, 2));
	elog.append(new HStimStartEvent(1, 1, 4));
	elog.append(new HStimStartEvent(1, 2, 5));
	elog.append(new HStimStartEvent(1, 3, 6));
	elog.append(new HTrialStartEvent(1, 0, 10));
	elog.append(new HLookEvent(HLook(LookLeft, 100, 1100), 1100));
	elog.append(new HLookEvent(HLook(LookLeft, 2200, 3200), 3200));
	
	// Phase log should still be empty as no trials have ended.
	plog = elog.getPhaseLog(QString("PreTest"));
	QVERIFY(plog.size()==0);
	QCOMPARE(plog.totalLookingTime(), 0);
	QVERIFY(!fixedNCriteria.isPhaseComplete(plog));
	QVERIFY(!totalLookingTimeCriteria.isPhaseComplete(plog));
	
	elog.append(new HLookEvent(HLook(LookLeft, 3300, 4300), 4300));
	elog.append(new HLookEvent(HLook(LookLeft, 4400, 5400), 5400));
	elog.append(new HTrialEndEvent());

	// Phase log should have 4000ms of looking
	plog = elog.getPhaseLog(QString("PreTest"));
	QVERIFY(plog.size()==1);
	QCOMPARE(plog.totalLookingTime(), 4000);
	QVERIFY(!fixedNCriteria.isPhaseComplete(plog));
	QVERIFY(!totalLookingTimeCriteria.isPhaseComplete(plog));

	elog.append(new HStimRequestEvent(1, 7000));
	elog.append(new HStimStartEvent(1, 1, 7001));
	elog.append(new HStimStartEvent(1, 2, 7002));
	elog.append(new HStimStartEvent(1, 3, 7003));
	elog.append(new HTrialStartEvent(2, 0, 7010));
	elog.append(new HLookEvent(HLook(LookLeft, 10100, 11100), 11100));
	elog.append(new HLookEvent(HLook(LookLeft, 12200, 13200), 13200));
	elog.append(new HLookEvent(HLook(LookLeft, 13300, 14300), 14300));
	elog.append(new HLookEvent(HLook(LookLeft, 14400, 15400), 15400));
	elog.append(new HTrialEndEvent());
	
	// Phase log should have 8000ms of looking
	plog = elog.getPhaseLog(QString("PreTest"));
	QVERIFY(plog.size()==2);
	QCOMPARE(plog.totalLookingTime(), 8000);
	QVERIFY(!fixedNCriteria.isPhaseComplete(plog));
	QVERIFY(!totalLookingTimeCriteria.isPhaseComplete(plog));

	
	elog.append(new HStimRequestEvent(1, 17000));
	elog.append(new HStimStartEvent(1, 1, 17001));
	elog.append(new HStimStartEvent(1, 2, 17002));
	elog.append(new HStimStartEvent(1, 3, 17003));
	elog.append(new HTrialStartEvent(3, 0, 17010));
	elog.append(new HLookEvent(HLook(LookLeft, 17100, 18100), 18100));
	elog.append(new HLookEvent(HLook(LookLeft, 18200, 19200), 19200));
	elog.append(new HTrialEndEvent(kTrialEndFail));
	
	// Phase log should have 8000ms of looking, with just 2 trials.
	// The last trial failed - should not count
	plog = elog.getPhaseLog(QString("PreTest"));
	QVERIFY(plog.size()==2);
	QCOMPARE(plog.totalLookingTime(), 8000);
	QVERIFY(!fixedNCriteria.isPhaseComplete(plog));
	QVERIFY(!totalLookingTimeCriteria.isPhaseComplete(plog));
	
	
	elog.append(new HStimRequestEvent(1, 20000));
	elog.append(new HStimStartEvent(1, 1, 20001));
	elog.append(new HStimStartEvent(1, 2, 20002));
	elog.append(new HStimStartEvent(1, 3, 20003));
	elog.append(new HTrialStartEvent(3, 1, 20010));
	elog.append(new HLookEvent(HLook(LookLeft, 20100, 21100)));
	elog.append(new HLookEvent(HLook(LookLeft, 22200, 23200)));
	elog.append(new HLookEvent(HLook(LookLeft, 23300, 24300)));
	elog.append(new HLookEvent(HLook(LookLeft, 24400, 25400)));
	elog.append(new HTrialEndEvent(kTrialEndSuccess, 25400));

	plog = elog.getPhaseLog();
	QVERIFY(plog.size()==3);
	QCOMPARE(plog.totalLookingTime(), 12000);
	QVERIFY(!fixedNCriteria.isPhaseComplete(plog));
	QVERIFY(totalLookingTimeCriteria.isPhaseComplete(plog));


	
	elog.append(new HStimRequestEvent(1, 30000));
	elog.append(new HStimStartEvent(1, 1, 30001));
	elog.append(new HStimStartEvent(1, 2, 30002));
	elog.append(new HStimStartEvent(1, 3, 30003));
	elog.append(new HTrialStartEvent(4, 0, 30010));
	elog.append(new HLookEvent(HLook(LookLeft, 30100, 31100)));
	elog.append(new HLookEvent(HLook(LookLeft, 32200, 33200)));
	elog.append(new HLookEvent(HLook(LookLeft, 33300, 34300)));
	elog.append(new HLookEvent(HLook(LookLeft, 34400, 35400)));
	elog.append(new HTrialEndEvent(kTrialEndSuccess, 35401));
	elog.append(new HPhaseEndEvent());

	plog = elog.getPhaseLog();
	QVERIFY(plog.size()==4);
	QCOMPARE(plog.totalLookingTime(), 16000);
	QVERIFY(fixedNCriteria.isPhaseComplete(plog));
	QVERIFY(totalLookingTimeCriteria.isPhaseComplete(plog));
	
	
	// Start habituation phase. 
	// Test all four habituation criteria here. 
	
	// Now add some events
	elog.append(new HPhaseStartEvent((char *)"Habituation", 100000));
	elog.append(new HStimRequestEvent(1, 100002));
	elog.append(new HStimStartEvent(1, 1, 100004));
	elog.append(new HStimStartEvent(1, 2, 100005));
	elog.append(new HStimStartEvent(1, 3, 100006));

	elog.append(new HTrialStartEvent(1, 0, 100010));
	elog.append(new HLookEvent(HLook(LookLeft, 101000, 102000), 102000));
	elog.append(new HTrialEndEvent(kTrialEndSuccess, 102000));

	plog = elog.getPhaseLog();
	QVERIFY(plog.size()==1);
	QCOMPARE(plog.totalLookingTime(), 1000);
	QVERIFY(!criteriaFirstNFixed.isPhaseComplete(plog));
	QVERIFY(!criteriaFirstNSliding.isPhaseComplete(plog));
	QVERIFY(!criteriaLongestNFixed.isPhaseComplete(plog));
	QVERIFY(!criteriaLongestNSliding.isPhaseComplete(plog));

	
	elog.append(new HTrialStartEvent(2, 0, 102000));
	elog.append(new HLookEvent(HLook(LookLeft, 102000, 104000), 104000));
	elog.append(new HTrialEndEvent(kTrialEndSuccess, 104000));

	plog = elog.getPhaseLog();
	QVERIFY(plog.size()==2);
	QCOMPARE(plog.totalLookingTime(), 3000);
	QVERIFY(!criteriaFirstNFixed.isPhaseComplete(plog));
	QVERIFY(!criteriaFirstNSliding.isPhaseComplete(plog));
	QVERIFY(!criteriaLongestNFixed.isPhaseComplete(plog));
	QVERIFY(!criteriaLongestNSliding.isPhaseComplete(plog));
	
	elog.append(new HTrialStartEvent(3, 0, 104000));
	elog.append(new HLookEvent(HLook(LookLeft, 104000, 107000), 107000));
	elog.append(new HTrialEndEvent(kTrialEndSuccess, 107000));

	plog = elog.getPhaseLog();
	QVERIFY(plog.size()==3);
	QCOMPARE(plog.totalLookingTime(), 6000);
	QVERIFY(!criteriaFirstNFixed.isPhaseComplete(plog));
	QVERIFY(!criteriaFirstNSliding.isPhaseComplete(plog));
	QVERIFY(!criteriaLongestNFixed.isPhaseComplete(plog));
	QVERIFY(!criteriaLongestNSliding.isPhaseComplete(plog));
	
	elog.append(new HTrialStartEvent(4, 0, 107000));
	elog.append(new HLookEvent(HLook(LookLeft, 107000, 108000), 108000));
	elog.append(new HTrialEndEvent(kTrialEndSuccess, 108000));

	plog = elog.getPhaseLog();
	QVERIFY(plog.size()==4);
	QCOMPARE(plog.totalLookingTime(), 7000);
	QVERIFY(!criteriaFirstNFixed.isPhaseComplete(plog));
	QVERIFY(!criteriaFirstNSliding.isPhaseComplete(plog));
	QVERIFY(!criteriaLongestNFixed.isPhaseComplete(plog));
	QVERIFY(!criteriaLongestNSliding.isPhaseComplete(plog));
	
	elog.append(new HTrialStartEvent(5, 0, 108000));
	elog.append(new HLookEvent(HLook(LookLeft, 108000, 110000), 110000));
	elog.append(new HTrialEndEvent(kTrialEndSuccess, 110000));

	plog = elog.getPhaseLog();
	QVERIFY(plog.size()==5);
	QCOMPARE(plog.totalLookingTime(), 9000);
	QVERIFY(!criteriaFirstNFixed.isPhaseComplete(plog));
	QVERIFY(!criteriaFirstNSliding.isPhaseComplete(plog));
	QVERIFY(!criteriaLongestNFixed.isPhaseComplete(plog));
	QVERIFY(!criteriaLongestNSliding.isPhaseComplete(plog));
	
	elog.append(new HTrialStartEvent(6, 0, 110000));
	elog.append(new HLookEvent(HLook(LookLeft, 110000, 112000), 112000));
	elog.append(new HTrialEndEvent(kTrialEndSuccess, 112000));

	plog = elog.getPhaseLog();
	QVERIFY(plog.size()==6);
	QCOMPARE(plog.totalLookingTime(), 11000);
	QVERIFY(!criteriaFirstNFixed.isPhaseComplete(plog));
	QVERIFY(!criteriaFirstNSliding.isPhaseComplete(plog));
	QVERIFY(!criteriaLongestNFixed.isPhaseComplete(plog));
	QVERIFY(!criteriaLongestNSliding.isPhaseComplete(plog));
	
	elog.append(new HTrialStartEvent(7, 0, 111000));
	elog.append(new HLookEvent(HLook(LookLeft, 111000, 112000), 112000));
	elog.append(new HTrialEndEvent(kTrialEndSuccess, 112000));

	plog = elog.getPhaseLog();
	QVERIFY(plog.size()==7);
	QCOMPARE(plog.totalLookingTime(), 12000);
	QVERIFY(!criteriaFirstNFixed.isPhaseComplete(plog));
	QVERIFY(!criteriaFirstNSliding.isPhaseComplete(plog));
	QVERIFY(!criteriaLongestNFixed.isPhaseComplete(plog));
	QVERIFY(!criteriaLongestNSliding.isPhaseComplete(plog));
	
	elog.append(new HTrialStartEvent(8, 0, 112000));
	elog.append(new HLookEvent(HLook(LookLeft, 112000, 113000), 113000));
	elog.append(new HTrialEndEvent(kTrialEndSuccess, 113000));

	plog = elog.getPhaseLog();
	QVERIFY(plog.size()==8);
	QCOMPARE(plog.totalLookingTime(), 13000);
	QVERIFY(!criteriaFirstNFixed.isPhaseComplete(plog));
	QVERIFY(!criteriaFirstNSliding.isPhaseComplete(plog));
	QVERIFY(!criteriaLongestNFixed.isPhaseComplete(plog));
	QVERIFY(!criteriaLongestNSliding.isPhaseComplete(plog));
	
	elog.append(new HTrialStartEvent(9, 0, 113000));
	elog.append(new HLookEvent(HLook(LookLeft, 113000, 113500), 113500));
	elog.append(new HTrialEndEvent(kTrialEndSuccess, 113500));

	plog = elog.getPhaseLog();
	QVERIFY(plog.size()==9);
	QCOMPARE(plog.totalLookingTime(), 13500);
	QVERIFY(criteriaFirstNFixed.isPhaseComplete(plog));
	QVERIFY(criteriaFirstNSliding.isPhaseComplete(plog));
	QVERIFY(criteriaLongestNFixed.isPhaseComplete(plog));
	QVERIFY(criteriaLongestNSliding.isPhaseComplete(plog));
	
}


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
