/*
 * HGMM.cpp
 *
 *  Created on: Jul 11, 2017
 *      Author: dan
 */

#include "HGMM.h"
#include "HGMMPipeline.h"
#include <QMapIterator>
#include <QEventLoop>
#include <gst/gst.h>
#include <iostream>


#define SPECIALPLAYHACK 1

//const Habit::StimulusSettings HGMM::dummyStimulusSettings;

HGMM::HGMM(HStimulusWidget *center, const QDir& dir, bool useISS, const QColor& bkgdColor, PipelineFactory factory)
: m_stimulusLayoutType(HStimulusLayoutType::HStimulusLayoutSingle)
, m_root(dir)
, m_bUseISS(useISS)
, m_pCenter(center)
, m_pLeft(NULL)
, m_pRight(NULL)
, m_pipelineCurrent(NULL)
, m_gthread(NULL)
, m_pgml(NULL)
, m_bReady(false)
, m_pipelineFactory(factory)
{
//	m_readyTimeout = new QTimer(this);
//	m_readyTimeout->setSingleShot(true);
//	m_readyCheck = new QTimer(this);
//	connect(m_readyTimeout, SIGNAL(timeout()), this, SLOT	(readyFail()));
//	connect(m_readyCheck, SIGNAL(timeout()), this, SLOT(readyCheck()));
	m_defaultKey = addStimulus(QString("default"), QColor(Qt::gray), -3);
	addBackground(bkgdColor);

	// launch main loop thread
	m_pgml = g_main_loop_new(NULL, FALSE);
	m_gthread = g_thread_new("HGMM-main-loop", &HGMM::threadFunc, m_pgml);
}

HGMM::HGMM(HStimulusWidget *left, HStimulusWidget *right, const QDir& dir, bool useISS, const QColor& bkgdColor, PipelineFactory factory)
: m_stimulusLayoutType(HStimulusLayoutType::HStimulusLayoutLeftRight)
, m_root(dir)
, m_bUseISS(useISS)
, m_pCenter(NULL)
, m_pLeft(left)
, m_pRight(right)
, m_pipelineCurrent(NULL)
, m_gthread(NULL)
, m_pgml(NULL)
, m_bReady(false)
, m_pipelineFactory(factory)
{
//	m_readyTimeout = new QTimer(this);
//	m_readyTimeout->setSingleShot(true);
//	m_readyCheck = new QTimer(this);
//	connect(m_readyTimeout, SIGNAL(timeout()), this, SIGNAL(readyFail()));
//	connect(m_readyCheck, SIGNAL(timeout()), this, SLOT(readyCheck()));
	m_defaultKey = addStimulus(QString("default"), QColor(Qt::gray), -3);
	addBackground(bkgdColor);

	// launch main loop thread
	m_pgml = g_main_loop_new(NULL, FALSE);
	m_gthread = g_thread_new("HGMM-main-loop", &HGMM::threadFunc, m_pgml);
}

gpointer HGMM::threadFunc(gpointer user_data)
{
	GMainLoop *pgml = (GMainLoop *)user_data;
	// create and start main loop here
	qDebug() << "starting main loop\n";
	g_main_loop_run(pgml);
	qDebug() << "main loop ended\n";
	return NULL;
}

HGMM::~HGMM()
{
	qDebug() << "~HGMM()";

	// exit main loop
	qDebug() << "quit main loop";
	g_main_loop_quit(m_pgml);
	qDebug() << "g_thread_join...";
	g_thread_join(m_gthread);
	qDebug() << "g_thread_join done\n";

	qDeleteAll(m_mapPipelines);
	m_mapPipelines.clear();
	qDebug() << "~HGMM() - done";
}

HStimulusWidget *HGMM::getHStimulusWidget(const HPlayerPositionType& type)
{
	if (type == HPlayerPositionType::Left)
		return m_pLeft;
	else if (type == HPlayerPositionType::Right)
		return m_pRight;
	else if (type == HPlayerPositionType::Center)
		return m_pCenter;
	else
		return NULL;
}

unsigned int HGMM::nextKey()
{
	return m_mapPipelines.size();
}

QList<unsigned int> HGMM::getContextStimList(int context)
{
	return m_mapContext.values(context);
}

void HGMM::addStimuli(const Habit::StimuliSettings& ss, int context)
{
	QListIterator<Habit::StimulusSettings> it(ss.stimuli());	// note: getting const HStimulusSettingsList&, it.next() will be const StimulusSettings&
	while (it.hasNext())
		addStimulus(it.next(), context);
	return;
}

unsigned int HGMM::addStimulus(unsigned int key, const Habit::StimulusSettings& stimulus, int context)
{
	HPipeline *pipeline;

	qDebug() << "Adding stimulus (key " << key << "): " << stimulus.getName() << " context " << context;

	pipeline = m_pipelineFactory(key, stimulus, m_root, m_stimulusLayoutType, true, m_bUseISS, this);
//	// Create a helper.
//	if (m_stimulusLayoutType == HStimulusLayoutType::HStimulusLayoutSingle)
//	{
//		pipeline = new HGMMPipeline(key, stimulus, m_root, m_pCenter->getHVideoWidget(), m_bUseISS, this);
//
//	}
//	else if (m_stimulusLayoutType == HStimulusLayoutType::HStimulusLayoutLeftRight)
//	{
//		pipeline = new HGMMPipeline(key, stimulus, m_root, m_pLeft->getHVideoWidget(), m_pRight->getHVideoWidget(), m_bUseISS, this);
//	}
//
//	// Add helper to map
	m_mapPipelines.insert(key, pipeline);

	// add key to context map
	m_mapContext.insert(context, key);

	return key;
}

unsigned int HGMM::addStimulus(unsigned int key, const QString& name, const QColor& color, int context)
{
	HPipeline *pipeline;
	Habit::StimulusSettings stimulus(name, color);

	qDebug() << "Adding solid color stimulus (key " << key << "): " << color << " context " << context;

	pipeline = m_pipelineFactory(key, stimulus, m_root, m_stimulusLayoutType, true, m_bUseISS, this);
//	// Create a helper.
//	if (m_stimulusLayoutType == HStimulusLayoutType::HStimulusLayoutSingle)
//	{
//		pipeline = new HGMMPipeline(key, stimulus, m_root, m_pCenter->getHVideoWidget(), m_bUseISS, this);
//	}
//	else if (m_stimulusLayoutType == HStimulusLayoutType::HStimulusLayoutLeftRight)
//	{
//		pipeline = new HGMMPipeline(key, stimulus, m_root, m_pLeft->getHVideoWidget(), m_pRight->getHVideoWidget(), m_bUseISS, this);
//	}
//
//	// Add helper to map
	m_mapPipelines.insert(key, pipeline);

	// add key to context map
	m_mapContext.insert(context, key);

	return key;
}


unsigned int HGMM::addStimulus(const Habit::StimulusSettings& ss, int context)
{
	return addStimulus(nextKey(), ss, context);
}

unsigned int HGMM::addStimulus(const QString& name, const QColor& color, int context)
{
	return addStimulus(nextKey(), name, color, context);
}

unsigned int HGMM::addAG(const Habit::StimulusSettings& ssAG)
{
	m_agKey = addStimulus(ssAG, -1);
	preroll(m_agKey);
	return m_agKey;
}

unsigned int HGMM::addBackground(const QColor& color)
{
	qWarning() << "TODO: ensure addBackground is called only once";
	m_backgroundKey = addStimulus(QString("background"), color, -2);
	preroll(m_backgroundKey);
	return(m_backgroundKey);
}

void HGMM::clear()
{
	stim(m_backgroundKey);
}

void HGMM::ag()
{
	m_bPendingAG = true;
	m_bPendingStim = false;
	m_iPendingStimKey = m_agKey;
	playStim(m_agKey);
}


void HGMM::nowPlaying()
{
	qDebug() << "HGMM::nowPlaying";
	if (m_bPendingAG)
	{
		qDebug() << "Q_EMIT(agStarted(m_iPendingStimKey))";
		Q_EMIT(agStarted(m_iPendingStimKey));
	}
	else if (m_bPendingStim)
	{
		qDebug() << "Q_EMIT(stimStarted(m_iPendingStimKey))";
		Q_EMIT(stimStarted(m_iPendingStimKey));
	}
}

void HGMM::stim(unsigned int key)
{
	//qDebug() << "HGMM::stim(" << key << ")";
	m_bPendingStim = true;
	m_bPendingAG = false;
	m_iPendingStimKey = key;
	playStim(key);
}

void HGMM::ready(unsigned int key)
{
	qDebug() << "ready " << key << " stim " << getStimulusSettings(key).getName();

	if (m_mapPipelines.contains(key))
	{
		m_mapPipelines.value(key)->ready();
	}
	else
	{
		qWarning() << "HGMM::ready(): key " << key << " not found!";
	}
}

void HGMM::preroll(unsigned int key)
{
	qDebug() << "preroll " << key << " stim " << getStimulusSettings(key).getName();
	if (m_mapPipelines.contains(key))
	{
		std::cout << "pipeline: " << *m_mapPipelines.value(key);
		m_mapPipelines.value(key)->preroll();
	}
	else
	{
		qWarning() << "HGMM::preroll(): key " << key << " not found!";
	}
}

void HGMM::pause(unsigned int key)
{
	if (m_mapPipelines.contains(key))
	{
		m_mapPipelines.value(key)->pause();
	}
	else
	{
		qWarning() << "HGMM::pause(): key " << key << " not found!";
	}
}

void HGMM::rewind(unsigned int key)
{
	if (m_mapPipelines.contains(key))
	{
		m_mapPipelines.value(key)->rewind();
	}
	else
	{
		qWarning() << "HGMM::rewind(): key " << key << " not found!";
	}
}

void HGMM::playStim(unsigned int key)
{
	qDebug() << "playstim " << key << " stim " << getStimulusSettings(key).getName();
	HPipeline *pipeline = NULL;		// the pipeline that will be played

	// get pipeline that will be displayed/played.
	// If the key is not found, that's bad. Use default.
	if (m_mapPipelines.contains(key))
	{
		pipeline = m_mapPipelines.value(key);
	}
	else
	{
		qWarning() << "key " << key << " not found! Using default stim.";
		pipeline = m_mapPipelines.value(m_defaultKey);
	}

	if (pipeline != m_pipelineCurrent)
	{

		// deal with current pipeline
		if (m_pipelineCurrent)
		{
			//qDebug() << "pause current stim";
			disconnect(m_pipelineCurrent, SIGNAL(nowPlaying()), this, SLOT(nowPlaying()));
			m_pipelineCurrent->pause();
			m_pipelineCurrent->detachWidgetsFromSinks();
		}

		// set things up for new pipeline
		connect(pipeline, SIGNAL(nowPlaying()), this, SLOT(nowPlaying()));
		if (m_stimulusLayoutType==HStimulusLayoutType::HStimulusLayoutSingle)
		{
			pipeline->attachWidgetsToSinks(m_pCenter->getHVideoWidget());
		}
		else if (m_stimulusLayoutType==HStimulusLayoutType::HStimulusLayoutLeftRight)
		{
			pipeline->attachWidgetsToSinks(m_pLeft->getHVideoWidget(), m_pRight->getHVideoWidget());
		}
	}

	pipeline->play();
	m_pipelineCurrent = pipeline;

	// update widget geometry
	updateGeometry();

	// let everybody know we just switched to a new sink
	Q_EMIT stimulusChanged();
}

void HGMM::stop()
{
	if (m_pipelineCurrent)
	{
		disconnect(m_pipelineCurrent, SIGNAL(nowPlaying()), this, SLOT(nowPlaying()));
		m_pipelineCurrent->pause();
		m_pipelineCurrent->detachWidgetsFromSinks();
	}
}

void HGMM::updateGeometry()
{
	// Create a helper.
	if (m_stimulusLayoutType == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		m_pCenter->getHVideoWidget()->updateGeometry();
	}
	else if (m_stimulusLayoutType == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		m_pLeft->getHVideoWidget()->updateGeometry();
		m_pRight->getHVideoWidget()->updateGeometry();
	}
}

//void HGMM::getReady(int ms)
//{
//	m_readyTimeout->start(ms);
//	m_readyCheck->start(500);
//
//	connect(m_readyCheck,  SIGNAL(timeout()), this, SLOT(readyCheck()) );
//	connect(m_readyTimeout, SIGNAL(timeout()), this, SLOT(readyFail()));
//	m_readyTimeout->start(ms);
//	m_readyCheck->start(500);
//
//	return;
//}

bool HGMM::waitForStimuliReady(int maxMS, int checkIntervalMS)
{
#if 1
	Q_UNUSED(maxMS);
	Q_UNUSED(checkIntervalMS);
	return true;
#else
	QTimer maxTimer;
	maxTimer.setSingleShot(true);
	maxTimer.setInterval(maxMS);
	QTimer checkTimer;
	checkTimer.setInterval(checkIntervalMS);
	QEventLoop loop;

	// when maxTimer fires, quit the loop and stop the check timer
	connect(&maxTimer, SIGNAL(timeout()), &loop, SLOT(quit()));
	connect(&maxTimer, SIGNAL(timeout()), &checkTimer, SLOT(stop()));

	// when check timer fires, call readyCheck, which checks all pipelines, and if all are ready it
	// will emit mmReady() and set m_bReady to true.
	connect(&checkTimer, SIGNAL(timeout()), this, SLOT(readyCheck()));

	// when mmReady() is emitted, stop loop and both timers
	connect(this, SIGNAL(mmReady()), &loop, SLOT(quit()));
	connect(this, SIGNAL(mmReady()), &maxTimer, SLOT(stop()));
	connect(this, SIGNAL(mmReady()), &checkTimer, SLOT(stop()));

	m_bReady = false;
	maxTimer.start();
	checkTimer.start();
	loop.exec();
	return m_bReady;
#endif
}

#if 0
void HGMM::readyCheck()
{
	qDebug() << "readyCheck():";
	//if (m_bReady) return;

	// iterate through helpers, check whether each has needed pads connected (and hence is prerolled)
	QMapIterator<unsigned int, HPipeline *> it(m_mapPipelines);
	while (it.hasNext())
	{
	    it.next();
	    if (!it.value()->hasPads()) return;
	}

	// only get here if all pipleines are ready
	m_bReady = true;
	qDebug() << "emit mmReady()";
	Q_EMIT mmReady();
}
#endif

const Habit::StimulusSettings HGMM::getStimulusSettings(unsigned int key) const
{
	static Habit::StimulusSettings dummy;
	if (m_mapStimulusSettings.contains(key))
		return m_mapStimulusSettings.value(key);
	else
		return dummy;
}
