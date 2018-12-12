#include "testmisc.h"
#include "HEventLog.h"
#include "HEvents.h"
#include "HTrialGenerator.h"
#include "HPhaseLog.h"
#include "HPhaseCriteria.h"
#include <QTemporaryFile>
//#include "monitorsettings.h"
#include "controlbaroptions.h"
//#include "trialsinfo.h"
//#include "designsettings.h"
//#include "habituationsettings.h"
#include "stimulusdisplayinfo.h"
#include "stimulussettings.h"
#include "attentiongettersettings.h"
#include "stimulisettings.h"
#include "experimentsettings.h"
#include "runsettings.h"
#include "subjectsettings.h"
#include <QBuffer>
#include <QDataStream>

#include "TestPreroller.h"

Habit::StimulusSettings TestHabutil::getSS()
{
	Habit::StimulusSettings ss0(QString("stim 0"));
	Habit::StimulusInfo left("left", QString("left/file/name"), true, 95);
	Habit::StimulusInfo center("center", QString("center/file/name"), true, 93);
	Habit::StimulusInfo right("right", QString("right/file/name"), true, 91);
	Habit::StimulusInfo iss("iss", QString("iss/file/name"), true, 97);
	ss0.setId(888);
	ss0.setLeftStimulusInfo(left);
	ss0.setCenterStimulusInfo(center);
	ss0.setRightStimulusInfo(right);
	ss0.setIndependentSoundInfo(iss);
	return ss0;
}

void TestHabutil::testEventLogRW()
{
	HPhaseStartEvent* eventPhaseStart = new HPhaseStartEvent(QString("TestPhase"), 0, 12345);
	HPhaseEndEvent* eventPhaseEnd = new HPhaseEndEvent(23456);
	HTrialStartEvent* eventTrialStart = new HTrialStartEvent(5, 8, 23459);
	HTrialEndEvent* eventTrialEnd = new HTrialEndEvent(HTrialEndType::HTrialEndGotLook, 9878);
	HAGRequestEvent* eventAGRequest = new HAGRequestEvent(3332);
	HAGStartEvent* eventAGStart = new HAGStartEvent(2, 8888);
	HStimRequestEvent* eventStimRequest = new HStimRequestEvent(3, 8122);
	HStimStartEvent* eventStimStart = new HStimStartEvent(1, 8234);
	Habit::StimulusSettings ss(getSS());
	HStimulusSettingsEvent* eventSS = new HStimulusSettingsEvent(ss, 5, 7654);
	HAttentionEvent* eventAttention = new HAttentionEvent(3499);
	HLookEvent* eventLook = new HLookEvent(HLook(HLookDirection::LookLeft, 2377, 3499), 3500);
	HLookTransEvent* eventLookTrans = new HLookTransEvent(HLookTrans::NoneCenter, 6633);
	HHabituationSuccessEvent* eventHabituationSuccess = new HHabituationSuccessEvent(8347);
	HHabituationFailureEvent* eventHabituationFailure = new HHabituationFailureEvent(8348);

    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    QDataStream out(&buffer);
    out << *eventPhaseStart;
    out << *eventPhaseEnd;
    out << *eventTrialStart;
    out << *eventTrialEnd;
    out << *eventAGRequest;
    out << *eventAGStart;
    out << *eventStimRequest;
    out << *eventStimStart;
    out << *eventSS;
    out << *eventAttention;
    out << *eventLook;
    out << *eventLookTrans;
    out << *eventHabituationSuccess;
    out << *eventHabituationFailure;
    buffer.close();


    buffer.open(QIODevice::ReadOnly);
    QDataStream in(&buffer);

    HEvent* pPhaseStart = NULL;
    HEvent* pPhaseEnd = NULL;
    HEvent* pTrialStart = NULL;
    HEvent* pTrialEnd = NULL;
    HEvent* pAGRequest = NULL;
    HEvent* pAGStart = NULL;
    HEvent* pStimRequest = NULL;
    HEvent* pStimStart = NULL;
    HEvent* pSS = NULL;
    HEvent* pAttention = NULL;
    HEvent* pLook = NULL;
    HEvent* pLookTrans = NULL;
    HEvent* pHabituationSuccess = NULL;
    HEvent* pHabituationFailure = NULL;

    pPhaseStart = HEvent::getEvent(in);
    QVERIFY(pPhaseStart);
    pPhaseEnd = HEvent::getEvent(in);
    QVERIFY(pPhaseEnd);
    pTrialStart = HEvent::getEvent(in);
    QVERIFY(pTrialStart);
    pTrialEnd = HEvent::getEvent(in);
    QVERIFY(pTrialEnd);
    pAGRequest = HEvent::getEvent(in);
    QVERIFY(pAGRequest);
    pAGStart = HEvent::getEvent(in);
    QVERIFY(pAGStart);
    pStimRequest = HEvent::getEvent(in);
    QVERIFY(pStimRequest);
    pStimStart = HEvent::getEvent(in);
    QVERIFY(pStimStart);
    pSS = HEvent::getEvent(in);
    QVERIFY(pSS);
    pAttention = HEvent::getEvent(in);
    QVERIFY(pAttention);
    pLook = HEvent::getEvent(in);
    QVERIFY(pLook);
    pLookTrans = HEvent::getEvent(in);
    QVERIFY(pLookTrans);
    pHabituationSuccess = HEvent::getEvent(in);
    QVERIFY(pHabituationSuccess);
    pHabituationFailure = HEvent::getEvent(in);
    QVERIFY(pHabituationFailure);
    buffer.close();

    QVERIFY(*eventPhaseStart == *static_cast<HPhaseStartEvent*>(pPhaseStart));
    QVERIFY(*eventPhaseEnd == *static_cast<HPhaseEndEvent*>(pPhaseEnd));
    QVERIFY(*eventTrialStart == *static_cast<HTrialStartEvent*>(pTrialStart));
    QVERIFY(*eventTrialEnd == *static_cast<HTrialEndEvent*>(pTrialEnd));
    QVERIFY(*eventAGRequest == *static_cast<HAGRequestEvent*>(pAGRequest));
    QVERIFY(*eventAGStart == *static_cast<HAGStartEvent*>(pAGStart));
    QVERIFY(*eventStimRequest == *static_cast<HStimRequestEvent*>(pStimRequest));
    QVERIFY(*eventStimStart == *static_cast<HStimStartEvent*>(pStimStart));
    QVERIFY(*eventSS == *static_cast<HStimulusSettingsEvent*>(pSS));
    QVERIFY(*eventAttention == *static_cast<HAttentionEvent*>(pAttention));
    QVERIFY(*eventLook == *static_cast<HLookEvent*>(pLook));
    QVERIFY(*eventLookTrans == *static_cast<HLookTransEvent*>(pLookTrans));
    QVERIFY(*eventHabituationSuccess == *static_cast<HHabituationSuccessEvent*>(pHabituationSuccess));
    QVERIFY(*eventHabituationFailure == *static_cast<HHabituationFailureEvent*>(pHabituationFailure));

    HEventLog log;
    log.append(eventPhaseStart);
    log.append(eventPhaseEnd);
    log.append(eventTrialStart);
    log.append(eventTrialEnd);
    log.append(eventAGRequest);
    log.append(eventAGStart);
    log.append(eventStimRequest);
    log.append(eventStimStart);
    log.append(eventSS);
    log.append(eventAttention);
    log.append(eventLook);
    log.append(eventLookTrans);
    log.append(eventHabituationSuccess);
    log.append(eventHabituationFailure);

    buffer.open(QIODevice::WriteOnly);
    QDataStream out2(&buffer);
    out2 << log;
    buffer.close();

    HEventLog log2;
    buffer.open(QIODevice::ReadOnly);
    QDataStream in2(&buffer);
    in2 >> log2;
    QVERIFY(log.size() == log2.size());
    QVERIFY(log == log2);

	QListIterator<HEvent*> events(log);
	events.toFront();
	HPhaseStartEvent* e1 = static_cast<HPhaseStartEvent*>(events.next());
	qDebug() << e1->eventCSV();
	QVERIFY(*e1 == *eventPhaseStart);
	HPhaseEndEvent* e2 = static_cast<HPhaseEndEvent*>(events.next());
	QVERIFY(*e2 == *eventPhaseEnd);
	QVERIFY(!(*e2 == *eventPhaseStart));
	HTrialStartEvent* e3 = static_cast<HTrialStartEvent*>(events.next());
	QVERIFY(*e3 == *eventTrialStart);
	HTrialEndEvent* e4 = static_cast<HTrialEndEvent*>(events.next());
	QVERIFY(*e4 == *eventTrialEnd);
	HAGRequestEvent* e5 = static_cast<HAGRequestEvent*>(events.next());
	QVERIFY(*e5 == *eventAGRequest);
	HAGStartEvent* e6 = static_cast<HAGStartEvent*>(events.next());
	QVERIFY(*e6 == *eventAGStart);
	HStimRequestEvent* e7 = static_cast<HStimRequestEvent*>(events.next());
	QVERIFY(*e7 == *eventStimRequest);
	HStimStartEvent* e8 = static_cast<HStimStartEvent*>(events.next());
	QVERIFY(*e8 == *eventStimStart);
	HStimulusSettingsEvent* e9 = static_cast<HStimulusSettingsEvent*>(events.next());
	QVERIFY(*e9 == *eventSS);
	HAttentionEvent* e10 = static_cast<HAttentionEvent*>(events.next());
	QVERIFY(*e10 == *eventAttention);
	HLookEvent* e11 = static_cast<HLookEvent*>(events.next());
	QVERIFY(*e11 == *eventLook);
	HLookTransEvent* e12 = static_cast<HLookTransEvent*>(events.next());
	QVERIFY(*e12 == *eventLookTrans);
	HHabituationSuccessEvent* e13 = static_cast<HHabituationSuccessEvent*>(events.next());
	QVERIFY(*e13 == *eventHabituationSuccess);
	HHabituationFailureEvent* e14 = static_cast<HHabituationFailureEvent*>(events.next());
	QVERIFY(*e14 == *eventHabituationFailure);


}


//void TestHabutil::testSubjectSettings()
//{
//	Habit::SubjectSettings ss;
//	ss.setId(345);
//	ss.setSubjectName(QString("John Smith"));
//	ss.setBirthDate(QDateTime(QDate(2013, 4, 30)));
//	ss.setTestDate(QDateTime(QDate(2013, 5, 1)));
//	ss.setCondition("whatever");
//	ss.setObserver("whomever");
//	ss.setCellNumber(QString("111-111-1111"));
//	ss.setComments(QString("Comments here"));
//
//	Habit::SubjectSettings ss1(ss);
//	QVERIFY(ss == ss1);
//
//	ss1.setTestDate(QDateTime(QDate(2012, 1, 31)));
//	QVERIFY(!(ss == ss1));
//
//    QByteArray byteArray;
//    QBuffer buffer(&byteArray);
//    buffer.open(QIODevice::WriteOnly);
//    QDataStream out(&buffer);
//    out << ss;
//    buffer.close();
//
//    Habit::SubjectSettings ss_readFromBuffer;
//    buffer.open(QIODevice::ReadOnly);
//    QDataStream in(&buffer);
//    in >> ss_readFromBuffer;
//    buffer.close();
//    QVERIFY(ss == ss_readFromBuffer);
//
//}


//void TestHabutil::testRunSettings()
//{
//	Habit::RunSettings rs;
//	rs.setId(234);
//	rs.setExperimentId(123);
//	rs.setSubjectId(945);
//	rs.setHabituationOrder(QString("1 2 3 4 5 6 7"));
//	rs.setHabituationRandomized(true);
//	rs.setHabituationRandomizeMethod(1);
//
//	rs.setPretestOrder(QString("1 2 3 4 5"));
//	rs.setPretestRandomized(false);
//	rs.setPretestRandomizeMethod(0);
//
//	rs.setTestOrder(QString("3 4 5 6 7"));
//	rs.setTestRandomized(false);
//	rs.setTestRandomizeMethod(1);
//
//	Habit::RunSettings rs1(rs);
//	QVERIFY(rs == rs1);
//
//	rs1.setPretestOrder(QString("2 30 12"));
//	QVERIFY(!(rs == rs1));
//
//    QByteArray byteArray;
//    QBuffer buffer(&byteArray);
//    buffer.open(QIODevice::WriteOnly);
//    QDataStream out(&buffer);
//    out << rs;
//    buffer.close();
//
//    Habit::RunSettings rs_readFromBuffer;
//    buffer.open(QIODevice::ReadOnly);
//    QDataStream in(&buffer);
//    in >> rs_readFromBuffer;
//    buffer.close();
//    QVERIFY(rs == rs_readFromBuffer);
//}

//void TestHabutil::testExperimentSettings()
//{
//	Habit::ExperimentSettings e0;
//	QString name("TestExperiment");
//
//    // Control Bar Options
//
//	Habit::ControlBarOptions cbo0;
//	cbo0.setId(998);
//	cbo0.setUseControlBar(true);
//    cbo0.setDisplayCurrentExperiment(true);
//    cbo0.setDisplayCurrentStimulus(false);
//
//    e0.setControlBarOptions(cbo0);
//
//	// stimulus display info
//
//	Habit::StimulusDisplayInfo sd0;
//	sd0.setId(4);
//	sd0.setDisplayType(HDisplayType::HDisplayTypeFullScreen);
//	sd0.setBackGroundColor(QColor(100, 200, 100));
//	sd0.setMaintainOriginalAspectRatio(true);
//	sd0.setPresentationStyle(HPresentationStyle::HPresentationStyleMonitorDefined);
//
//	e0.setStimulusDisplayInfo(sd0);
//
//	// attention getter
//
//	Habit::AttentionGetterSettings ags;
//	Habit::StimulusSettings ss0("ags", HStimContext::AttentionGetter);
//	Habit::StimulusInfo left("left", QString("left/file/name"), true, 95);
//	Habit::StimulusInfo center("center", QString("center/file/name"), true, 93);
//	Habit::StimulusInfo right("right", QString("right/file/name"), true, 91);
//	Habit::StimulusInfo iss("iss", QString("iss/file/name"), true, 97);
//	ss0.setId(888);
//	ss0.setLeftEnabled(true);
//	ss0.setLeftStimulusInfo(left);
//	ss0.setCenterEnabled(true);
//	ss0.setCenterStimulusInfo(center);
//	ss0.setRightEnabled(true);
//	ss0.setRightStimulusInfo(right);
//	ss0.setIndependentSoundEnabled(true);
//	ss0.setIndependentSoundInfo(iss);
//	ags.setAttentionGetterStimulus(ss0);
//	ags.setId(832);
//	ags.setBackGroundColor(QColor(123, 111, 109));
//	ags.setUseAttentionGetter(true);
//
//	e0.setAttentionGetterSettings(ags);
//
//	// pretest stimuli - leave along (default constructor?)
//
//
//	// habituation stimuli
//	Habit::StimuliSettings hstimuli(HStimContext::HabituationPhase);
//	Habit::StimulusSettings ss("1", HStimContext::HabituationPhase);
//	ss.setId(456);
//	ss.setLeftEnabled(true);
//	ss.setLeftStimulusInfo(Habit::StimulusInfo("1.left", "1.left/file/name", true, 23));
//	ss.setRightEnabled(true);
//	ss.setRightStimulusInfo(Habit::StimulusInfo("1.right", "1.right/file/name", true, 24));
//	ss.setCenterEnabled(true);
//	ss.setCenterStimulusInfo(Habit::StimulusInfo("1.center", "1.center/file/name", true, 25));
//	ss.setIndependentSoundEnabled(true);
//	ss.setIndependentSoundInfo(Habit::StimulusInfo("1.iss", "1.iss/file/name", true, 26));
//	hstimuli.addStimuli(ss);
//
//	ss.setId(234);
//	ss.setIndependentSoundEnabled(false);
//	ss.setRightStimulusInfo(Habit::StimulusInfo("2.right", "file/name/aaa", false, 20));
//	hstimuli.addStimuli(ss);
//
//	e0.setHabituationStimuliSettings(hstimuli);
//
//	// test stimuli
//	Habit::StimuliSettings tstimuli(HStimContext::HabituationPhase);
//	ss.setId(456);
//	ss.setLeftEnabled(true);
//	ss.setLeftStimulusInfo(Habit::StimulusInfo("1.left", "1.left/file/name/test", true, 25));
//	ss.setRightEnabled(true);
//	ss.setRightStimulusInfo(Habit::StimulusInfo("1.right", "1.right/file/name/test", true, 724));
//	ss.setCenterEnabled(true);
//	ss.setCenterStimulusInfo(Habit::StimulusInfo("1.center", "1.center/file/name/test", true, 285));
//	ss.setIndependentSoundEnabled(true);
//	ss.setIndependentSoundInfo(Habit::StimulusInfo("1.iss", "1.iss/file/name/test", true, 26));
//	tstimuli.addStimuli(ss);
//
//	ss.setId(234);
//	ss.setIndependentSoundEnabled(false);
//	ss.setRightStimulusInfo(Habit::StimulusInfo("2.right", "file/name/aaaaaaa", false, 20));
//	tstimuli.addStimuli(ss);
//
//	e0.setPreTestStimuliSettings(tstimuli);
//
//
//	// NOW start tests....
//	Habit::ExperimentSettings e1(e0);
//	QVERIFY(e0 == e1);
//
//
//	m0.setLeftMonitorIndex(222);
//	e1.setMonitorSettings(m0);
//	QVERIFY(!(e0 == e1));
//
//    QByteArray byteArray;
//    QBuffer buffer(&byteArray);
//    buffer.open(QIODevice::WriteOnly);
//    QDataStream out(&buffer);
//    out << e0;
//    buffer.close();
//
//    Habit::ExperimentSettings e_readFromBuffer;
//    buffer.open(QIODevice::ReadOnly);
//    QDataStream in(&buffer);
//    in >> e_readFromBuffer;
//    buffer.close();
//    QVERIFY(e0 == e_readFromBuffer);
//}
//
//void TestHabutil::testStimuliSettings()
//{
//	Habit::StimuliSettings stimuli(HStimContext::HabituationPhase);
//	Habit::StimulusSettings ss("1", HStimContext::HabituationPhase);
//	ss.setId(456);
//	ss.setLeftEnabled(true);
//	ss.setLeftStimulusInfo(Habit::StimulusInfo("1.left", "1.left/file/name", true, 23));
//	ss.setRightEnabled(true);
//	ss.setRightStimulusInfo(Habit::StimulusInfo("1.right", "1.right/file/name", true, 24));
//	ss.setCenterEnabled(true);
//	ss.setCenterStimulusInfo(Habit::StimulusInfo("1.center", "1.center/file/name", true, 25));
//	ss.setIndependentSoundEnabled(true);
//	ss.setIndependentSoundInfo(Habit::StimulusInfo("1.iss", "1.iss/file/name", true, 26));
//	stimuli.addStimuli(ss);
//
//	ss.setId(234);
//	ss.setIndependentSoundEnabled(false);
//	ss.setRightStimulusInfo(Habit::StimulusInfo("2.right", "file/name/aaa", false, 20));
//	stimuli.addStimuli(ss);
//
//	Habit::StimuliSettings stimuli1(stimuli);
//	QVERIFY(stimuli == stimuli1);
//
//	ss.setId(876);
//	stimuli1.addStimuli(ss);
//	QVERIFY(!(stimuli == stimuli1));
//
//    QByteArray byteArray;
//    QBuffer buffer(&byteArray);
//    buffer.open(QIODevice::WriteOnly);
//    QDataStream out(&buffer);
//    out << stimuli;
//    buffer.close();
//
//    Habit::StimuliSettings ss_readFromBuffer(HStimContext::TestPhase);
//    buffer.open(QIODevice::ReadOnly);
//    QDataStream in(&buffer);
//    in >> ss_readFromBuffer;
//    buffer.close();
//    QVERIFY(stimuli == ss_readFromBuffer);
//
//
//}
//
//void TestHabutil::testAttentionGetterSettings()
//{
//	Habit::AttentionGetterSettings ags;
//	Habit::StimulusSettings ss0("ags", HStimContext::AttentionGetter);
//	Habit::StimulusInfo left("left", QString("left/file/name"), true, 95);
//	Habit::StimulusInfo center("center", QString("center/file/name"), true, 93);
//	Habit::StimulusInfo right("right", QString("right/file/name"), true, 91);
//	Habit::StimulusInfo iss("iss", QString("iss/file/name"), true, 97);
//	ss0.setId(888);
//	ss0.setLeftEnabled(true);
//	ss0.setLeftStimulusInfo(left);
//	ss0.setCenterEnabled(true);
//	ss0.setCenterStimulusInfo(center);
//	ss0.setRightEnabled(true);
//	ss0.setRightStimulusInfo(right);
//	ss0.setIndependentSoundEnabled(true);
//	ss0.setIndependentSoundInfo(iss);
//	ags.setAttentionGetterStimulus(ss0);
//	ags.setId(832);
//	ags.setBackGroundColor(QColor(123, 111, 109));
//	ags.setUseAttentionGetter(true);
//
//	Habit::AttentionGetterSettings ags1(ags);
//	QVERIFY(ags == ags1);
//
//	ags1.setUseAttentionGetter(false);
//	QVERIFY(!(ags == ags1));
//
//    QByteArray byteArray;
//    QBuffer buffer(&byteArray);
//    buffer.open(QIODevice::WriteOnly);
//    QDataStream out(&buffer);
//    out << ags;
//    buffer.close();
//
//    Habit::AttentionGetterSettings ags_readFromBuffer;
//    buffer.open(QIODevice::ReadOnly);
//    QDataStream in(&buffer);
//    in >> ags_readFromBuffer;
//    buffer.close();
//    QVERIFY(ags == ags_readFromBuffer);
//
//}
//
//
//void TestHabutil::testStimulusInfo()
//{
//	Habit::StimulusInfo si0;
//	si0.setAudioBalance(23);
//	si0.setFileName(QString("file/name/path"));
//	si0.setLoopPlayBack(true);
//	si0.setName("test");
//
//	Habit::StimulusInfo si1(si0);
//	QVERIFY(si0 == si1);
//
//	si1.setLoopPlayBack(false);
//	QVERIFY(!(si0 == si1));
//
//    QByteArray byteArray;
//    QBuffer buffer(&byteArray);
//    buffer.open(QIODevice::WriteOnly);
//    QDataStream out(&buffer);
//    out << si0;
//    buffer.close();
//
//    Habit::StimulusInfo s_readFromBuffer;
//    buffer.open(QIODevice::ReadOnly);
//    QDataStream in(&buffer);
//    in >> s_readFromBuffer;
//    buffer.close();
//    QVERIFY(si0 == s_readFromBuffer);
//
//}
//
//void TestHabutil::testStimulusSettings()
//{
//	Habit::StimulusSettings ss0("test", HStimContext::PreTestPhase);
//	Habit::StimulusInfo left("left", QString("left/file/name"), true, 95);
//	Habit::StimulusInfo center("center", QString("center/file/name"), true, 93);
//	Habit::StimulusInfo right("right", QString("right/file/name"), true, 91);
//	Habit::StimulusInfo iss("iss", QString("iss/file/name"), true, 97);
//	ss0.setId(888);
//	ss0.setLeftEnabled(true);
//	ss0.setLeftStimulusInfo(left);
//	ss0.setCenterEnabled(true);
//	ss0.setCenterStimulusInfo(center);
//	ss0.setRightEnabled(true);
//	ss0.setRightStimulusInfo(right);
//	ss0.setIndependentSoundEnabled(true);
//	ss0.setIndependentSoundInfo(iss);
//
//	Habit::StimulusSettings ss1(ss0);
//	QVERIFY(ss0 == ss1);
//
//	center.setAudioBalance(34);
//	ss1.setCenterStimulusInfo(center);
//	QVERIFY(!(ss0 == ss1));
//
//    QByteArray byteArray;
//    QBuffer buffer(&byteArray);
//    buffer.open(QIODevice::WriteOnly);
//    QDataStream out(&buffer);
//    out << ss0;
//    buffer.close();
//
//    Habit::StimulusSettings ss_readFromBuffer;
//    buffer.open(QIODevice::ReadOnly);
//    QDataStream in(&buffer);
//    in >> ss_readFromBuffer;
//    buffer.close();
//
//    QVERIFY(ss0 == ss_readFromBuffer);
//
//}
//
//void TestHabutil::testStimulusDisplayInfo()
//{
//	Habit::StimulusDisplayInfo s0;
//	s0.setId(4);
//	s0.setDisplayType(HDisplayType::HDisplayTypeFullScreen);
//	s0.setBackGroundColor(QColor(100, 200, 100));
//	s0.setMaintainOriginalAspectRatio(true);
//	s0.setPresentationStyle(HPresentationStyle::HPresentationStyleMonitorDefined);
//	Habit::StimulusDisplayInfo s1(s0);
//
//	QVERIFY(s0 == s1);
//	s1.setBackGroundColor(QColor(200, 100, 200));
//	QVERIFY(!(s0 == s1));
//
//
//    QByteArray byteArray;
//    QBuffer buffer(&byteArray);
//    buffer.open(QIODevice::WriteOnly);
//    QDataStream out(&buffer);
//    out << s0;
//    buffer.close();
//
//    Habit::StimulusDisplayInfo s_readFromBuffer;
//    buffer.open(QIODevice::ReadOnly);
//    QDataStream in(&buffer);
//    in >> s_readFromBuffer;
//    buffer.close();
//    QVERIFY(s0 == s_readFromBuffer);
//
//}
//
//void TestHabutil::testCriterionSettings()
//{
//	Habit::CriterionSettings c0;
//	c0.setBasis(HCriterionBasisType::HCriterionBasisFirstN);
//	c0.setPercent(44);
//	c0.setWindowSize(6);
//	c0.setWindowType(HCriterionWindowType::HCriterionWindowSliding);
//
//	Habit::CriterionSettings c1(c0);
//	QVERIFY(c0 == c1);
//
//	c1.setWindowType(HCriterionWindowType::HCriterionWindowFixed);
//	QVERIFY(!(c0 == c1));
//
//    QByteArray byteArray;
//    QBuffer buffer(&byteArray);
//    buffer.open(QIODevice::WriteOnly);
//    QDataStream out(&buffer);
//    out << c0;
//    buffer.close();
//
//    Habit::CriterionSettings c_readFromBuffer;
//    buffer.open(QIODevice::ReadOnly);
//    QDataStream in(&buffer);
//    in >> c_readFromBuffer;
//    buffer.close();
//    QVERIFY(c0 == c_readFromBuffer);
//}
//
//
//void TestHabutil::testHabituationSettings()
//{
//	Habit::HabituationSettings h0;
//	h0.setId(2);
//	h0.setTotalLookLengthToEnd(234);
//	h0.setHabituationType(HHabituationType::HHabituationTypeFixedN);
//	Habit::CriterionSettings c0;
//	c0.setBasis(HCriterionBasisType::HCriterionBasisLongestN);
//	c0.setPercent(44);
//	c0.setWindowSize(6);
//	c0.setWindowType(HCriterionWindowType::HCriterionWindowFixed);
//	h0.setCriterionSettings(c0);
//
//	Habit::HabituationSettings h1(h0);
//	QVERIFY(h0 == h1);
//
//	c0.setWindowSize(2);
//	h1.setCriterionSettings(c0);
//	QVERIFY(!(h0 == h1));
//
//    QByteArray byteArray;
//    QBuffer buffer(&byteArray);
//    buffer.open(QIODevice::WriteOnly);
//    QDataStream out(&buffer);
//    out << h0;
//    buffer.close();
//
//    Habit::HabituationSettings h_readFromBuffer;
//    buffer.open(QIODevice::ReadOnly);
//    QDataStream in(&buffer);
//    in >> h_readFromBuffer;
//    buffer.close();
//    QVERIFY(h0 == h_readFromBuffer);
//}
//
//void TestHabutil::testDesignSettings()
//{
//	Habit::DesignSettings d0;
//	Habit::TrialsInfo pti, hti, tti;
//	d0.setId(99);
//	pti.setTrialCompletionType(HTrialCompletionType::HTrialCompletionSubjectControlled);
//	pti.setLength(333);
//	pti.setLookTimes(222);
//	pti.setNumberOfTrials(5);
//	d0.setPretestTrialsInfo(pti);
//
//	hti.setTrialCompletionType(HTrialCompletionType::HTrialCompletionSubjectControlled);
//	hti.setLength(337);
//	hti.setLookTimes(2255);
//	hti.setNumberOfTrials(3);
//	d0.setHabituationTrialsInfo(hti);
//
//	tti.setTrialCompletionType(HTrialCompletionType::HTrialCompletionSubjectControlled);
//	tti.setLength(555);
//	tti.setLookTimes(467);
//	tti.setNumberOfTrials(7);
//	d0.setTestTrialsInfo(tti);
//
//	Habit::DesignSettings d1(d0);
//	QVERIFY(d0 == d1);
//
//	hti.setNumberOfTrials(9);
//	d1.setHabituationTrialsInfo(hti);
//	QVERIFY(!(d0 == d1));
//
//    QByteArray byteArray;
//    QBuffer buffer(&byteArray);
//    buffer.open(QIODevice::WriteOnly);
//    QDataStream out(&buffer);
//    out << d0;
//    buffer.close();
//
//    Habit::DesignSettings d_readFromBuffer;
//    buffer.open(QIODevice::ReadOnly);
//    QDataStream in(&buffer);
//    in >> d_readFromBuffer;
//    buffer.close();
//    QVERIFY(d0 == d_readFromBuffer);
//
//
//}
//
//
//void TestHabutil::testTrialsInfo()
//{
//	Habit::TrialsInfo h0;
//	h0.setTrialCompletionType(HTrialCompletionType::HTrialCompletionFixedLength);
//	h0.setLength(876);
//	h0.setLookTimes(237);
//	h0.setNumberOfTrials(8);
//	Habit::TrialsInfo h1(h0);
//	QVERIFY(h0 == h1);
//
//	h1.setNumberOfTrials(7);
//	QVERIFY(!(h0 == h1));
//
//    QByteArray byteArray;
//    QBuffer buffer(&byteArray);
//    buffer.open(QIODevice::WriteOnly);
//    QDataStream out(&buffer);
//    out << h0;
//    buffer.close();
//
//    Habit::TrialsInfo h_readFromBuffer;
//    buffer.open(QIODevice::ReadOnly);
//    QDataStream in(&buffer);
//    in >> h_readFromBuffer;
//    buffer.close();
//    QVERIFY(h0 == h_readFromBuffer);
//
//
//}
//
//void TestHabutil::testMonitorSettings()
//{
//	Habit::MonitorSettings m0;
//	m0.setId(999);
//	m0.setControlMonitorIndex(3);
//    m0.setLeftMonitorIndex(2);
//    m0.setCenterMonitorIndex(1);
//    m0.setRightMonitorIndex(0);
//    Habit::MonitorSettings m1(m0);
//    QVERIFY(m0 == m1);
//
//    m1.setRightMonitorIndex(9);
//    QVERIFY(!(m0 == m1));
//
//
//    QByteArray byteArray;
//    QBuffer buffer(&byteArray);
//    buffer.open(QIODevice::WriteOnly);
//    QDataStream out(&buffer);
//    out << m0;
//    buffer.close();
//
//    Habit::MonitorSettings m_readFromBuffer;
//    buffer.open(QIODevice::ReadOnly);
//    QDataStream in(&buffer);
//    in >> m_readFromBuffer;
//    buffer.close();
//    QVERIFY(m0 == m_readFromBuffer);
//
//}
//
//void TestHabutil::testControlBarOptions()
//{
//	Habit::ControlBarOptions c0;
//	c0.setId(998);
//	c0.setUseControlBar(true);
//    c0.setDisplayCurrentExperiment(true);
//    c0.setDisplayCurrentStimulus(false);
//    Habit::ControlBarOptions c1(c0);
//    QVERIFY(c0 == c1);
//
//    c1.setDisplayCurrentStimulus(true);
//    QVERIFY(!(c0 == c1));
//
//
//    QByteArray byteArray;
//    QBuffer buffer(&byteArray);
//    buffer.open(QIODevice::WriteOnly);
//    QDataStream out(&buffer);
//    out << c0;
//    buffer.close();
//
//    Habit::ControlBarOptions c_readFromBuffer;
//    buffer.open(QIODevice::ReadOnly);
//    QDataStream in(&buffer);
//    in >> c_readFromBuffer;
//    buffer.close();
//    QVERIFY(c0 == c_readFromBuffer);
//
//}

TestHabutil::TestHabutil() : QObject()
{};

void TestHabutil::testEventLog()
{
	int eventNumber;
	HEventLog elog;
	HPhaseLog plog;
	bool isHabituated;
	HPhaseFixedNCriteria fixedN4Criteria(4);
	HPhaseTotalLookingTimeCriteria totalLookingTime10Criteria(10000, 4);
	HPhaseTotalLookingTimeCriteria totalLookingTime10FailCriteria(20000, 4);
	Habit::CriterionSettings csFirstNFixed(HCriterionBasisType::HCriterionBasisFirstN, 50, 3, HCriterionWindowType::HCriterionWindowFixed);
	HPhaseHabituationCriteria criteriaFirstNFixed(csFirstNFixed, 10);
	Habit::CriterionSettings csLongestNFixed(HCriterionBasisType::HCriterionBasisLongestN, 50, 3, HCriterionWindowType::HCriterionWindowFixed);
	HPhaseHabituationCriteria criteriaLongestNFixed(csLongestNFixed, 10);
	Habit::CriterionSettings csFirstNSliding(HCriterionBasisType::HCriterionBasisFirstN, 50, 3, HCriterionWindowType::HCriterionWindowSliding);
	HPhaseHabituationCriteria criteriaFirstNSliding(csFirstNSliding, 10);
	Habit::CriterionSettings csLongestNSliding(HCriterionBasisType::HCriterionBasisLongestN, 50, 3, HCriterionWindowType::HCriterionWindowSliding);
	HPhaseHabituationCriteria criteriaLongestNSliding(csLongestNSliding, 10);

	
	// test empty event log
	plog = elog.getPhaseLog(QString("Test0"));
	QVERIFY(plog.size()==0);
	QCOMPARE(plog.totalLookingTime(), 0);
	QVERIFY(!fixedN4Criteria.isPhaseComplete(plog, isHabituated, eventNumber));
	QVERIFY(!totalLookingTime10Criteria.isPhaseComplete(plog, isHabituated, eventNumber));

	plog = elog.getPhaseLog(QString(""));
	QVERIFY(plog.size()==0);
	QCOMPARE(plog.totalLookingTime(), 0);
	QVERIFY(!fixedN4Criteria.isPhaseComplete(plog, isHabituated, eventNumber));
	QVERIFY(!totalLookingTime10Criteria.isPhaseComplete(plog, isHabituated, eventNumber));
	
	// Now add some events. A stim and trial start, followed by two looks.
	// Trial not ended yet.

	elog.append(new HPhaseStartEvent(QString("Test0"), 1, 1));
	elog.append(new HStimRequestEvent(1, 2));
	elog.append(new HStimStartEvent(1, 4));
	elog.append(new HTrialStartEvent(1, 0, 10));
	elog.append(new HLookEvent(HLook(HLookDirection::LookLeft, 100, 1100), 1100));
	elog.append(new HLookEvent(HLook(HLookDirection::LookLeft, 2200, 3200), 3200));
	
	// Phase log should still be empty as no trials have ended.

	plog = elog.getPhaseLog(QString("Test0"));
	QVERIFY(plog.size()==0);
	QCOMPARE(plog.totalLookingTime(), 0);
	QVERIFY(!fixedN4Criteria.isPhaseComplete(plog, isHabituated, eventNumber));
	QVERIFY(!totalLookingTime10Criteria.isPhaseComplete(plog, isHabituated, eventNumber));
	

	// Add two more looks, and a trial end event.
	elog.append(new HLookEvent(HLook(HLookDirection::LookLeft, 3300, 4300), 4300));
	elog.append(new HLookEvent(HLook(HLookDirection::LookLeft, 4400, 5400), 5400));
	elog.append(new HTrialEndEvent(HTrialEndType::HTrialEndMaxAccumulatedLookTime, 5400));

	// Phase log should have 4000ms of looking, 1 trial
	plog = elog.getPhaseLog(QString("Test0"));

	QCOMPARE(plog.size(), 1);
	QCOMPARE(plog.totalLookingTime(), 4000);
	QVERIFY(!fixedN4Criteria.isPhaseComplete(plog, isHabituated, eventNumber));
	QVERIFY(!totalLookingTime10Criteria.isPhaseComplete(plog, isHabituated, eventNumber));


	// Second trial.
	elog.append(new HStimRequestEvent(1, 7000));
	elog.append(new HStimStartEvent(1, 7001));
	elog.append(new HTrialStartEvent(2, 0, 7010));
	elog.append(new HLookEvent(HLook(HLookDirection::LookLeft, 10100, 11100), 11100));
	elog.append(new HLookEvent(HLook(HLookDirection::LookLeft, 12200, 13200), 13200));
	elog.append(new HLookEvent(HLook(HLookDirection::LookLeft, 13300, 14300), 14300));
	elog.append(new HLookEvent(HLook(HLookDirection::LookLeft, 14400, 15400), 15400));
	elog.append(new HTrialEndEvent(HTrialEndType::HTrialEndMaxStimulusTime, 20000));
	
	// Phase log should have 8000ms of looking, 2 trials
	plog = elog.getPhaseLog(QString("Test0"));
	QVERIFY(plog.size()==2);
	QCOMPARE(plog.totalLookingTime(), 8000);
	QVERIFY(!fixedN4Criteria.isPhaseComplete(plog, isHabituated, eventNumber));
	QVERIFY(!totalLookingTime10Criteria.isPhaseComplete(plog, isHabituated, eventNumber));


	// Third trial, end with HTrialEndNoLookTimeout. That's a failure condition, so this trial should be excluded
	// from the phase log.
	elog.append(new HStimRequestEvent(1, 17000));
	elog.append(new HStimStartEvent(1, 17001));
	elog.append(new HTrialStartEvent(3, 0, 17010));
	elog.append(new HLookEvent(HLook(HLookDirection::LookLeft, 17100, 18100), 18100));
	elog.append(new HLookEvent(HLook(HLookDirection::LookLeft, 18200, 19200), 19200));
	elog.append(new HTrialEndEvent(HTrialEndType::HTrialEndNoLookTimeout, 20000));
	
	// Phase log should have 8000ms of looking, with just 2 trials.
	// The last trial failed - should not count
	plog = elog.getPhaseLog(QString("Test0"));
	QCOMPARE(plog.size(), 2);
	QCOMPARE(plog.totalLookingTime(), 8000);
	QVERIFY(!fixedN4Criteria.isPhaseComplete(plog, isHabituated, eventNumber));
	QVERIFY(!totalLookingTime10Criteria.isPhaseComplete(plog, isHabituated, eventNumber));
	
	
	// Fourth trial.
	elog.append(new HStimRequestEvent(1, 20000));
	elog.append(new HStimStartEvent(1, 20001));
	elog.append(new HTrialStartEvent(3, 1, 20010));
	elog.append(new HLookEvent(HLook(HLookDirection::LookLeft, 20100, 21100)));
	elog.append(new HLookEvent(HLook(HLookDirection::LookLeft, 22200, 23200)));
	elog.append(new HLookEvent(HLook(HLookDirection::LookLeft, 23300, 24300)));
	elog.append(new HTrialEndEvent(HTrialEndType::HTrialEndMaxStimulusTime, 29000));

	// There should be three entries (the third trial failed, remember), 11000
	plog = elog.getPhaseLog();
	QVERIFY(plog.size()==3);
	QCOMPARE(plog.totalLookingTime(), 11000);
	QVERIFY(!fixedN4Criteria.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	QVERIFY(totalLookingTime10Criteria.isPhaseComplete(plog, isHabituated, eventNumber) && isHabituated);
	QVERIFY(!totalLookingTime10FailCriteria.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);


	// One more trial.
	elog.append(new HStimRequestEvent(1, 30000));
	elog.append(new HStimStartEvent(1, 30001));
	elog.append(new HTrialStartEvent(4, 0, 30010));
	elog.append(new HLookEvent(HLook(HLookDirection::LookLeft, 30100, 31100)));
	elog.append(new HLookEvent(HLook(HLookDirection::LookLeft, 32200, 33200)));
	elog.append(new HLookEvent(HLook(HLookDirection::LookLeft, 33300, 34300)));
	elog.append(new HLookEvent(HLook(HLookDirection::LookLeft, 34400, 35400)));
	elog.append(new HTrialEndEvent(HTrialEndType::HTrialEndMaxStimulusTime, 35401));
	elog.append(new HPhaseEndEvent());

	plog = elog.getPhaseLog();
	QVERIFY(plog.size()==4);
	QCOMPARE(plog.totalLookingTime(), 15000);
	QVERIFY(fixedN4Criteria.isPhaseComplete(plog, isHabituated, eventNumber) && isHabituated);
	QVERIFY(totalLookingTime10Criteria.isPhaseComplete(plog, isHabituated, eventNumber) && isHabituated);
	QVERIFY(totalLookingTime10FailCriteria.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);	// complete, not habituated
	
	
	// Start habituation phase. 
	// Test all four habituation criteria here. 
	
	// Now add some events
	elog.append(new HPhaseStartEvent(QString("Habituation"), 1, 100000));
	elog.append(new HStimRequestEvent(1, 100002));
	elog.append(new HStimStartEvent(1, 100004));
	elog.append(new HStimStartEvent(2, 100005));
	elog.append(new HStimStartEvent(3, 100006));

	elog.append(new HTrialStartEvent(1, 0, 100010));
	elog.append(new HLookEvent(HLook(HLookDirection::LookLeft, 101000, 102000), 102000));
	elog.append(new HTrialEndEvent(HTrialEndType::HTrialEndGotLook, 102000));

	plog = elog.getPhaseLog();
	QVERIFY(plog.size()==1);
	QCOMPARE(plog.totalLookingTime(), 1000);
	QVERIFY(!criteriaFirstNFixed.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	QVERIFY(!criteriaFirstNSliding.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	QVERIFY(!criteriaLongestNFixed.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	QVERIFY(!criteriaLongestNSliding.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);

	
	elog.append(new HTrialStartEvent(2, 0, 102000));
	elog.append(new HLookEvent(HLook(HLookDirection::LookLeft, 102000, 104000), 104000));
	elog.append(new HTrialEndEvent(HTrialEndType::HTrialEndGotLook, 104000));

	plog = elog.getPhaseLog();
	QVERIFY(plog.size()==2);
	QCOMPARE(plog.totalLookingTime(), 3000);
	QVERIFY(!criteriaFirstNFixed.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	QVERIFY(!criteriaFirstNSliding.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	QVERIFY(!criteriaLongestNFixed.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	QVERIFY(!criteriaLongestNSliding.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	
	elog.append(new HTrialStartEvent(3, 0, 104000));
	elog.append(new HLookEvent(HLook(HLookDirection::LookLeft, 104000, 107000), 107000));
	elog.append(new HTrialEndEvent(HTrialEndType::HTrialEndGotLook, 107000));

	plog = elog.getPhaseLog();
	QVERIFY(plog.size()==3);
	QCOMPARE(plog.totalLookingTime(), 6000);
	QVERIFY(!criteriaFirstNFixed.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	QVERIFY(!criteriaFirstNSliding.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	QVERIFY(!criteriaLongestNFixed.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	QVERIFY(!criteriaLongestNSliding.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	
	elog.append(new HTrialStartEvent(4, 0, 107000));
	elog.append(new HLookEvent(HLook(HLookDirection::LookLeft, 107000, 108000), 108000));
	elog.append(new HTrialEndEvent(HTrialEndType::HTrialEndGotLook, 108000));

	plog = elog.getPhaseLog();
	QVERIFY(plog.size()==4);
	QCOMPARE(plog.totalLookingTime(), 7000);
	QVERIFY(!criteriaFirstNFixed.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	QVERIFY(!criteriaFirstNSliding.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	QVERIFY(!criteriaLongestNFixed.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	QVERIFY(!criteriaLongestNSliding.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	
	elog.append(new HTrialStartEvent(5, 0, 108000));
	elog.append(new HLookEvent(HLook(HLookDirection::LookLeft, 108000, 110000), 110000));
	elog.append(new HTrialEndEvent(HTrialEndType::HTrialEndGotLook, 110000));

	plog = elog.getPhaseLog();
	QVERIFY(plog.size()==5);
	QCOMPARE(plog.totalLookingTime(), 9000);
	QVERIFY(!criteriaFirstNFixed.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	QVERIFY(!criteriaFirstNSliding.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	QVERIFY(!criteriaLongestNFixed.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	QVERIFY(!criteriaLongestNSliding.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	
	elog.append(new HTrialStartEvent(6, 0, 110000));
	elog.append(new HLookEvent(HLook(HLookDirection::LookLeft, 110000, 112000), 112000));
	elog.append(new HTrialEndEvent(HTrialEndType::HTrialEndGotLook, 112000));

	plog = elog.getPhaseLog();
	QVERIFY(plog.size()==6);
	QCOMPARE(plog.totalLookingTime(), 11000);
	QVERIFY(!criteriaFirstNFixed.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	QVERIFY(!criteriaFirstNSliding.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	QVERIFY(!criteriaLongestNFixed.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	QVERIFY(!criteriaLongestNSliding.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	
	elog.append(new HTrialStartEvent(7, 0, 111000));
	elog.append(new HLookEvent(HLook(HLookDirection::LookLeft, 111000, 112000), 112000));
	elog.append(new HTrialEndEvent(HTrialEndType::HTrialEndGotLook, 112000));

	plog = elog.getPhaseLog();
	QVERIFY(plog.size()==7);
	QCOMPARE(plog.totalLookingTime(), 12000);
	QVERIFY(!criteriaFirstNFixed.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	QVERIFY(!criteriaFirstNSliding.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	QVERIFY(!criteriaLongestNFixed.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	QVERIFY(!criteriaLongestNSliding.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	
	elog.append(new HTrialStartEvent(8, 0, 112000));
	elog.append(new HLookEvent(HLook(HLookDirection::LookLeft, 112000, 113000), 113000));
	elog.append(new HTrialEndEvent(HTrialEndType::HTrialEndGotLook, 113000));

	plog = elog.getPhaseLog();
	QVERIFY(plog.size()==8);
	QCOMPARE(plog.totalLookingTime(), 13000);
	QVERIFY(!criteriaFirstNFixed.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	QVERIFY(!criteriaFirstNSliding.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	QVERIFY(!criteriaLongestNFixed.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	QVERIFY(!criteriaLongestNSliding.isPhaseComplete(plog, isHabituated, eventNumber) && !isHabituated);
	
	elog.append(new HTrialStartEvent(9, 0, 113000));
	elog.append(new HLookEvent(HLook(HLookDirection::LookLeft, 113000, 113500), 113500));
	elog.append(new HTrialEndEvent(HTrialEndType::HTrialEndGotLook, 113500));

	plog = elog.getPhaseLog();
	QVERIFY(plog.size()==9);
	QCOMPARE(plog.totalLookingTime(), 13500);
	QVERIFY(criteriaFirstNFixed.isPhaseComplete(plog, isHabituated, eventNumber) && isHabituated);
	QVERIFY(criteriaFirstNSliding.isPhaseComplete(plog, isHabituated, eventNumber) && isHabituated);
	QVERIFY(criteriaLongestNFixed.isPhaseComplete(plog, isHabituated, eventNumber) && isHabituated);
	QVERIFY(criteriaLongestNSliding.isPhaseComplete(plog, isHabituated, eventNumber) && isHabituated);
	
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


QTEST_MAIN(TestPreroller);
//#include "testmisc.moc"

//int main(int argc, char **argv)
//{
//	   int status = 0;
//	   {
//	      TestHabutil tc;
//	      status |= QTest::qExec(&tc, argc, argv);
//	   }
//	   {
//	      TestPreroller tc;
//	      status |= QTest::qExec(&tc, argc, argv);
//	   }
//}
