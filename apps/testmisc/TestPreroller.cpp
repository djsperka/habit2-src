/*
 * TestPreroller.cpp
 *
 *  Created on: Oct 29, 2018
 *      Author: dan
 */

#include "TestPreroller.h"
#include "HGMM.h"
#include "HWorkspaceUtil.h"
#include "HExperimentUtil.h"
#include "runsettings.h"
#include "HDBException.h"
#include <QtDebug>

TestPreroller::TestPreroller(): preroller(2) {
	// TODO Auto-generated constructor stub

}

TestPreroller::~TestPreroller() {
	// TODO Auto-generated destructor stub
}

#define PTS_FOR_PHASE_TRIAL(a,b) _PTS(QString(a), b, m_mapStimidLists.value(a).at(b))
#define STIMID_FOR_PHASE_TRIAL(a,b) m_mapStimidLists.value(QString(a)).at(b)

void TestPreroller::test1()
{
	QList<_PTS> l = preroller.getNextTrials(QString("phase1"));

	qDebug() << "getNextTrials(phase1)";
	qDebug() << l;

	// should get trial 0, 1 from phase "phase1"
	QCOMPARE(l.size(), 2);
	QCOMPARE(l.at(0), PTS_FOR_PHASE_TRIAL("phase1", 0));
	QCOMPARE(l.at(1), PTS_FOR_PHASE_TRIAL("phase1", 1));
}

void TestPreroller::test2()
{
	QList<_PTS> l = preroller.getNextTrials(QString("phase2"));

	qDebug() << "getNextTrials(phase2)";
	qDebug() << l;

	// should get trial 0, 1 from phase "phase2", and trial 0,1 from "phase3"
	QCOMPARE(l.size(), 4);
	QCOMPARE(l.at(0), PTS_FOR_PHASE_TRIAL("phase2", 0));
	QCOMPARE(l.at(1), PTS_FOR_PHASE_TRIAL("phase2", 1));
	QCOMPARE(l.at(2), PTS_FOR_PHASE_TRIAL("phase3", 0));
	QCOMPARE(l.at(3), PTS_FOR_PHASE_TRIAL("phase3", 1));

}

void TestPreroller::test3()
{
	QList<_PTS> l = preroller.getNextTrials(QString("phase1"), 0);

	qDebug() << "getNextTrials(phase1, 0)";
	qDebug() << l;

	// should get trial 1, 2 from phase "phase1"
	QCOMPARE(l.size(), 2);
	QCOMPARE(l.at(0), PTS_FOR_PHASE_TRIAL("phase1", 1));
	QCOMPARE(l.at(1), PTS_FOR_PHASE_TRIAL("phase1", 2));
}

void TestPreroller::test4()
{
	QList<_PTS> l = preroller.getNextTrials(QString("phase1"), 1);

	qDebug() << "getNextTrials(phase1, 1)";
	qDebug() << l;

	// should get trial 0, 1 from phase "phase1"
	QCOMPARE(l.size(), 4);
	QCOMPARE(l.at(0), PTS_FOR_PHASE_TRIAL("phase1", 2));
	QCOMPARE(l.at(1), PTS_FOR_PHASE_TRIAL("phase2", 0));
	QCOMPARE(l.at(2), PTS_FOR_PHASE_TRIAL("phase3", 0));
	QCOMPARE(l.at(3), PTS_FOR_PHASE_TRIAL("phase3", 1));
}

void TestPreroller::test5()
{
	QList<_PTS> l = preroller.getNextTrials(QString("phase1"), 2);

	qDebug() << "getNextTrials(phase1, 2)";
	qDebug() << l;

	// should get trial 0, 1 from phase "phase1"
	QCOMPARE(l.size(), 4);
	QCOMPARE(l.at(0), PTS_FOR_PHASE_TRIAL("phase2", 0));
	QCOMPARE(l.at(1), PTS_FOR_PHASE_TRIAL("phase2", 1));
	QCOMPARE(l.at(2), PTS_FOR_PHASE_TRIAL("phase3", 0));
	QCOMPARE(l.at(3), PTS_FOR_PHASE_TRIAL("phase3", 1));
}

bool TestPreroller::spyContainsStimid(const QSignalSpy& spy, unsigned int stimid)
{
	bool bfound = false;
//	QList<QVariant> arguments;
	for (int i=0; !bfound && i<spy.count(); i++)
		bfound = (spy.at(i).at(0).toUInt() == stimid);
	return bfound;
}

void TestPreroller::test6()
{

	QSignalSpy prerollingSpy(&HGMM::instance(), SIGNAL(prerolling(int)));
	QSignalSpy prerolledSpy(&HGMM::instance(), SIGNAL(prerolled(int)));

	// first, preroll the phase itself. This call should get us 2 prerolled stimuli...
	qDebug() << "Update preroll list for phase1";

	preroller.updatePrerollList("phase1");
	preroller.printStatus("p1/* prerolled");
	QTRY_VERIFY_WITH_TIMEOUT(prerolledSpy.count() >= 4, 10000);	// the 4 accounts for the background and default


	// compare prerolling spy to expected args : stimid for first, second stim
	QCOMPARE(prerollingSpy.count(), 2);
	QVERIFY(spyContainsStimid(prerollingSpy, STIMID_FOR_PHASE_TRIAL("phase1", 0)));
	QVERIFY(spyContainsStimid(prerollingSpy, STIMID_FOR_PHASE_TRIAL("phase1", 1)));

	// compare prerolled spy to same
	QCOMPARE(prerolledSpy.count(), 4);
	QVERIFY(spyContainsStimid(prerolledSpy, STIMID_FOR_PHASE_TRIAL("phase1", 0)));
	QVERIFY(spyContainsStimid(prerolledSpy, STIMID_FOR_PHASE_TRIAL("phase1", 1)));

	// compare pending preroll list to empty
	QVERIFY(preroller.getPendingPrerolledList().empty());

	// compare prerolled list to same stimids _PTS
	QVERIFY(preroller.getPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase1", 0)));
	QVERIFY(preroller.getPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase1", 1)));
	QVERIFY(preroller.getPrerolledList().size() == 2);

	// Now trial 0 has started...
	prerollingSpy.clear();
	prerolledSpy.clear();
	preroller.updatePrerollList("phase1", 0);
	preroller.printStatus("p1/0 started");
	QTRY_VERIFY_WITH_TIMEOUT(prerolledSpy.count() == 1, 2000);

	// test spy
	QVERIFY(spyContainsStimid(prerollingSpy, STIMID_FOR_PHASE_TRIAL("phase1", 2)));
	QVERIFY(spyContainsStimid(prerolledSpy, STIMID_FOR_PHASE_TRIAL("phase1", 2)));

	// preroller internal lists
	foreach(auto pts, preroller.getPendingPrerolledList())
	{
		qDebug() << pts;
	}
	QVERIFY(preroller.getPendingPrerolledList().empty());
	QVERIFY(preroller.getPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase1", 1)));
	QVERIFY(preroller.getPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase1", 2)));
	QVERIFY(preroller.getPrerolledList().size() == 2);



	// trial 0 ended, trial 1 starts
	prerollingSpy.clear();
	prerolledSpy.clear();
	preroller.addToCleanupList(STIMID_FOR_PHASE_TRIAL("phase1", 0));
	preroller.updatePrerollList("phase1", 1);
	preroller.printStatus("p1/0 done, p1/1 started");
	QTRY_VERIFY_WITH_TIMEOUT(prerolledSpy.count() == 3, 2000);

	// spy
	QVERIFY(spyContainsStimid(prerollingSpy, STIMID_FOR_PHASE_TRIAL("phase2", 0)));
	QVERIFY(spyContainsStimid(prerollingSpy, STIMID_FOR_PHASE_TRIAL("phase3", 0)));
	QVERIFY(spyContainsStimid(prerollingSpy, STIMID_FOR_PHASE_TRIAL("phase3", 1)));
	QVERIFY(spyContainsStimid(prerolledSpy, STIMID_FOR_PHASE_TRIAL("phase2", 0)));
	QVERIFY(spyContainsStimid(prerolledSpy, STIMID_FOR_PHASE_TRIAL("phase3", 0)));
	QVERIFY(spyContainsStimid(prerolledSpy, STIMID_FOR_PHASE_TRIAL("phase3", 1)));

	// preroller lists
	QVERIFY(preroller.getPendingPrerolledList().empty());
	QVERIFY(preroller.getPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase1", 2)));
	QVERIFY(preroller.getPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase2", 0)));
	QVERIFY(preroller.getPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase3", 0)));
	QVERIFY(preroller.getPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase3", 1)));

	// trial 1 ended, trial 2 starts
	prerollingSpy.clear();
	prerolledSpy.clear();
	preroller.addToCleanupList(STIMID_FOR_PHASE_TRIAL("phase1", 1));
	preroller.updatePrerollList("phase1", 2);
	preroller.printStatus("p1/1 done, p1/2 started");
	QTRY_VERIFY_WITH_TIMEOUT(prerolledSpy.count() == 1, 2000);

	// spy
	QVERIFY(spyContainsStimid(prerollingSpy, STIMID_FOR_PHASE_TRIAL("phase2", 1)));
	QVERIFY(spyContainsStimid(prerolledSpy, STIMID_FOR_PHASE_TRIAL("phase2", 1)));

	// preroller lists
	QVERIFY(preroller.getPendingPrerolledList().empty());
	QVERIFY(preroller.getPrerolledList().size() == 4);
	QVERIFY(preroller.getPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase2", 0)));
	QVERIFY(preroller.getPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase2", 1)));
	QVERIFY(preroller.getPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase3", 0)));
	QVERIFY(preroller.getPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase3", 1)));

	// trial 2 ended, phase ended, phase2/trial0 starts
	prerollingSpy.clear();
	prerolledSpy.clear();
	preroller.addToCleanupList(STIMID_FOR_PHASE_TRIAL("phase1", 2));
	preroller.addToCleanupList(QString("phase1"));
	preroller.updatePrerollList("phase2", 0);
	preroller.printStatus("p1/* done, p2/0 started");
	QTRY_VERIFY_WITH_TIMEOUT(prerolledSpy.count() == 1, 2000);

	// spy
	QVERIFY(spyContainsStimid(prerollingSpy, STIMID_FOR_PHASE_TRIAL("phase2", 2)));
	QVERIFY(spyContainsStimid(prerolledSpy, STIMID_FOR_PHASE_TRIAL("phase2", 2)));

	// preroller lists
	QVERIFY(preroller.getPendingPrerolledList().empty());
	QVERIFY(preroller.getPrerolledList().size() == 4);
	QVERIFY(preroller.getPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase2", 1)));
	QVERIFY(preroller.getPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase2", 2)));
	QVERIFY(preroller.getPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase3", 0)));
	QVERIFY(preroller.getPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase3", 1)));



	// p2/trial0 ended, p2/trial1 starts
	prerollingSpy.clear();
	prerolledSpy.clear();
	preroller.addToCleanupList(STIMID_FOR_PHASE_TRIAL("phase2", 0));
	preroller.updatePrerollList("phase2", 1);
	preroller.printStatus("p2/0 done, p2/1 started");
	QTRY_VERIFY_WITH_TIMEOUT(prerolledSpy.count() == 1, 2000);

	// spy
	QVERIFY(spyContainsStimid(prerollingSpy, STIMID_FOR_PHASE_TRIAL("phase2", 3)));
	QVERIFY(spyContainsStimid(prerolledSpy, STIMID_FOR_PHASE_TRIAL("phase2", 3)));

	// preroller lists
	QVERIFY(preroller.getPendingPrerolledList().empty());
	QVERIFY(preroller.getPrerolledList().size() == 4);
	QVERIFY(preroller.getPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase2", 2)));
	QVERIFY(preroller.getPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase2", 3)));
	QVERIFY(preroller.getPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase3", 0)));
	QVERIFY(preroller.getPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase3", 1)));




	// p2/1 ended, p2 ended, p3/0 starts
	prerollingSpy.clear();
	prerolledSpy.clear();
	preroller.addToCleanupList(STIMID_FOR_PHASE_TRIAL("phase2", 1));
	preroller.addToCleanupList("phase2");
	preroller.updatePrerollList("phase3", 0);
	preroller.printStatus("p2/* done, p3/0 started");
	QTRY_VERIFY_WITH_TIMEOUT(prerolledSpy.count() == 0, 2000);

	// spy
	QVERIFY(prerollingSpy.size()==0);
	QVERIFY(prerolledSpy.size()==0);

	// preroller lists
	QVERIFY(preroller.getPendingPrerolledList().size()==1);
	QVERIFY(preroller.getPendingPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase3", 2)));
	QVERIFY(preroller.getPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase3", 1)));





	// p3/0 ended, p3/1 starts
	prerollingSpy.clear();
	prerolledSpy.clear();
	preroller.addToCleanupList(STIMID_FOR_PHASE_TRIAL("phase3", 0));
	preroller.updatePrerollList("phase3", 1);
	preroller.printStatus("p3/0 done, p3/1 started");
	QTRY_VERIFY_WITH_TIMEOUT(prerolledSpy.count() == 1, 2000);

	// spy
	// The stim from p3/0 is also due to be prerolled as p3/2, hence it should have been
	// rewound, not prerolled. No "prerolling" signal, just a "prerolled" signal.
	QVERIFY(prerollingSpy.size()==0);
	QVERIFY(prerolledSpy.size()==1);

	// preroller lists
	QVERIFY(preroller.getPendingPrerolledList().size()==1);
	QVERIFY(preroller.getPendingPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase3", 3)));
	QVERIFY(preroller.getPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase3", 2)));




	// p3/1 ended, p3/2 starts
	prerollingSpy.clear();
	prerolledSpy.clear();
	preroller.addToCleanupList(STIMID_FOR_PHASE_TRIAL("phase3", 1));
	preroller.updatePrerollList("phase3", 2);
	preroller.printStatus("p3/1 done, p3/2 started");
	QTRY_VERIFY_WITH_TIMEOUT(prerolledSpy.count() == 1, 2000);

	// spy
	// The stim from p3/1 is also due to be prerolled as p3/3, hence it should have been
	// rewound, not prerolled. No "prerolling" signal, just a "prerolled" signal.
	QVERIFY(prerollingSpy.size()==0);
	QVERIFY(prerolledSpy.size()==1);

	// preroller lists
	QVERIFY(preroller.getPendingPrerolledList().size()==1);
	QVERIFY(preroller.getPendingPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase3", 4)));
	QVERIFY(preroller.getPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase3", 3)));




	// p3/2 ended, p3/3 starts
	// Note p3/4+ are all same stim.
	prerollingSpy.clear();
	prerolledSpy.clear();
	preroller.addToCleanupList(STIMID_FOR_PHASE_TRIAL("phase3", 2));
	preroller.updatePrerollList("phase3", 3);
	preroller.printStatus("p3/2 done, p3/3 started");
	QTRY_VERIFY_WITH_TIMEOUT(prerolledSpy.count() == 1, 2000);

	// spy
	// The stim from p3/2 is also due to be prerolled as p3/4, hence it should have been
	// rewound, not prerolled. No "prerolling" signal, just a "prerolled" signal.
	QVERIFY(prerollingSpy.size()==0);
	QVERIFY(prerolledSpy.size()==1);

	// preroller lists
	QVERIFY(preroller.getPendingPrerolledList().size()==1);
	QVERIFY(preroller.getPendingPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase3", 5)));
	QVERIFY(preroller.getPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase3", 4)));







	// p3/3 ended, p3/4 starts
	// Note p3/4+ are all same stim.
	prerollingSpy.clear();
	prerolledSpy.clear();
	preroller.addToCleanupList(STIMID_FOR_PHASE_TRIAL("phase3", 3));
	preroller.updatePrerollList("phase3", 4);
	preroller.printStatus("p3/3 done, p3/4 started");
	QTRY_VERIFY_WITH_TIMEOUT(prerolledSpy.count() == 0, 2000);

	// spy
	QVERIFY(prerollingSpy.size()==0);
	QVERIFY(prerolledSpy.size()==0);

	// preroller lists
	QVERIFY(preroller.getPendingPrerolledList().size()==2);
	QVERIFY(preroller.getPendingPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase3", 5)));
	QVERIFY(preroller.getPendingPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase3", 6)));
	QVERIFY(preroller.getPrerolledList().empty());



	// p3/3 ended, p3/4 starts
	// Note p3/4+ are all same stim.
	prerollingSpy.clear();
	prerolledSpy.clear();
	preroller.addToCleanupList(STIMID_FOR_PHASE_TRIAL("phase3", 4));
	preroller.updatePrerollList("phase3", 5);
	preroller.printStatus("p3/4 done, p3/5 started");
	QTRY_VERIFY_WITH_TIMEOUT(prerolledSpy.count() == 1, 2000);
	QVERIFY(spyContainsStimid(prerolledSpy, STIMID_FOR_PHASE_TRIAL("phase3", 5)));

	// spy
	QVERIFY(prerollingSpy.size()==0);
	QVERIFY(prerolledSpy.size()==1);

	// preroller lists
	QVERIFY(preroller.getPendingPrerolledList().size()==2);
	QVERIFY(preroller.getPendingPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase3", 6)));
	QVERIFY(preroller.getPendingPrerolledList().contains(PTS_FOR_PHASE_TRIAL("phase3", 7)));
	QVERIFY(preroller.getPrerolledList().size()==0);

}


void TestPreroller::initTestCase()
{
	qDebug() << "initTestCase()";
//	QList<int> l;
//	l.append(10);
//	l.append(11);
//	l.append(12);
//	preroller.addPhase(QString("phase1"), l, false);
//	l.clear();
//	l.append(20);
//	l.append(21);
//	l.append(22);
//	preroller.addPhase(QString("phase2"), l, true);
//	l.clear();
//	l.append(30);
//	l.append(31);
//	l.append(32);
//	preroller.addPhase(QString("phase3"), l, false);

	// Init gstreamer
	gst_init(NULL, NULL);

	// initialize workspace
	habutilSetWorkspace(QString("/Users/dan/Desktop/HTESTING"));
	//habutilSelectWorkspace();
	QVERIFY(habutilInitWorkspace());


	// Load experiment settings for test
	Habit::ExperimentSettings settings;
	try
	{
		settings.loadFromDB(QString("testPreroll"));
	}
	catch (const Habit::HDBException& e)
	{
		qCritical() << "Cannot load experiment \"testPreroll\" from database.";
		qCritical() << e.what();
		QVERIFY(false);
	}

	// initialize mm, populate with stimuli and ag
	QDir rootDir = habutilGetStimulusRootDir();
	HGMM::instance().reset(settings, rootDir);

	// Now populate mm. First, iterate over phases. For each phase, add stimuli to mm.
	QListIterator<Habit::HPhaseSettings> phaseIterator = settings.phaseIterator();
	while (phaseIterator.hasNext())
	{
		const Habit::HPhaseSettings& ps = phaseIterator.next();
		QList<unsigned int> stimidList;

		// I'm going to make up a special order for the third trial.

		if (ps.getName() != "phase3")
		{
			// Get the context stim list. It is a list of keys (used to call pmm->stim(key), e.g.)
			// in the order that they were added to the stim list, which is the same as the
			// order they are pulled from the DB.
			// When the actual order of presentation is generated (stimidListOrdered), the algorithm
			// need only generate a 0-based list -- the elements of that list are used to fetch the actual
			// MM keys to use in the experiment.
			stimidList = HGMM::instance().getContextStimList(ps.getSeqno());
		}
		else
		{
			QList< QPair<int, QString> > plist;
			QList<unsigned int> idList;
			if (!ps.stimuli().getIndexedOrderList(QString("o1"), plist))
				QVERIFY(false);
			idList = HGMM::instance().getContextStimList(ps.getSeqno());
			for (const auto& p: plist)
			{
				stimidList.append(idList.at(p.first));
			}
		}
		m_mapStimidLists.insert(ps.getName(), stimidList);
		foreach(int i, stimidList)
		{
			qDebug() << "seqno " << ps.getSeqno() << ":" << i;
		}

		bool bHab = !(ps.habituationSettings().getHabituationType()==HHabituationType::HHabituationTypeFixedN);
		preroller.addPhase(ps.getName(), stimidList, bHab);
	}

}

void TestPreroller::cleanupTestCase()
{
	qDebug() << "cleanupTestCase()";
}

void TestPreroller::init()
{
	qDebug() << "init called";
}

void TestPreroller::cleanup()
{
	qDebug() << "cleanup called";
}
