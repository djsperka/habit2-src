/*
 * HGMM.cpp
 *
 *  Created on: Jul 11, 2017
 *      Author: dan
 */

#include "HGMM.h"
#include "HPhaseSettings.h"
#include <QMapIterator>
#include <QEventLoop>
#include <QDialog>
#include <gst/gst.h>
#include <iostream>
#include <algorithm>


HGMM& HGMM::instance()
{
	static HGMM mm;
	return mm;
}

HGMM::HGMM(PipelineFactory factory)
: m_sdinfo()
//, m_bUseISS(false)
, m_pCenter(NULL)
, m_pLeft(NULL)
, m_pRight(NULL)
, m_pipelineCurrent(NULL)
, m_gthread(NULL)
, m_pgml(NULL)
, m_pipelineFactory(factory)
, m_backgroundKey(0)
, m_agKey(0)
, m_defaultKey(0)
, m_bPendingAG(false)
, m_bPendingStim(false)
, m_iPendingStimKey(0)
{
	// launch main loop thread
	m_pgml = g_main_loop_new(NULL, FALSE);
	m_gthread = g_thread_new("HGMM-main-loop", &HGMM::threadFunc, m_pgml);
}

void HGMM::reset()
{
	qDebug() << "HGMM::reset()";
	cleanupAll();
	qDeleteAll(m_mapPipelines);
	m_mapPipelines.clear();
	m_mapContext.clear();

	qDebug() << "map has " << m_mapPipelines.count() << "keys";
	m_pCenter = m_pLeft = m_pRight = NULL;
	m_pipelineCurrent = NULL;
	m_sdinfo = Habit::StimulusDisplayInfo();
}

void HGMM::cleanupAll()
{
	QMapIterator<unsigned int, HPipeline *> it(m_mapPipelines);
	while (it.hasNext())
	{
	    it.next();
	    it.value()->cleanup();
	}

	// cleanup static pipelines. These are not reconfigured here.
	HStaticStimPipeline *p;
	p = dynamic_cast<HStaticStimPipeline *>(m_mapPipelines[m_backgroundKey]);
	if (p)
	{
		p->forceCleanup();
	}
	p = dynamic_cast<HStaticStimPipeline *>(m_mapPipelines[m_defaultKey]);
	if (p)
	{
		p->forceCleanup();
	}
	p = dynamic_cast<HStaticStimPipeline *>(m_mapPipelines[m_agKey]);
	if (p)
	{
		p->forceCleanup();
	}
	return;
}
void HGMM::reset(const Habit::ExperimentSettings& settings, const QDir& dir)
{
	qDebug() << "HGMM::reset(const Habit::ExperimentSettings& settings, const QDir& dir) - layout type " << settings.getStimulusDisplayInfo().getStimulusLayoutType().name();
	reset();
	reset(settings.getStimulusDisplayInfo(), dir);

//	// Need to know if AG is used. If it is, add attention getter settings to media manager
//	if (settings.getAttentionGetterSettings().isAttentionGetterUsed() || settings.getAttentionGetterSettings().isFixedISI())
//	{
	qDebug() << "add ag";
	qDebug() << settings.getAttentionGetterSettings().getAttentionGetterStimulus();
	qDebug() << "sound only? " << settings.getAttentionGetterSettings().isSoundOnly();
		addAG(settings.getAttentionGetterSettings().getAttentionGetterStimulus(), settings.getAttentionGetterSettings().isSoundOnly());
//	}

	QListIterator<Habit::HPhaseSettings> phaseIterator = settings.phaseIterator();
	while (phaseIterator.hasNext())
	{
		const Habit::HPhaseSettings& ps = phaseIterator.next();
		if (ps.getIsEnabled())
		{
			addStimuli(ps.stimuli(), ps.getSeqno());
		}
	}
}

void HGMM::reset(const Habit::StimulusDisplayInfo& info, const QDir& dir)
{
	qDebug() << "HGMM::reset(" << info << ", " << dir << " )";
	m_sdinfo = info;
	m_root = dir;

	// if default key doesn't exist yet, create that pipleline. If it exists, then reconfigure it.
	qDebug() << "m_mapPipelines has " << m_mapPipelines.count() << " def " << m_defaultKey << "?" << m_mapPipelines.contains(m_defaultKey);
	if (m_mapPipelines.contains(m_defaultKey))
	{
		m_mapPipelines.value(m_defaultKey)->cleanup();
		m_mapPipelines.value(m_defaultKey)->reconfigure(m_sdinfo);
	}
	else
	{
		m_defaultKey = addStimulus(QString("default"), QColor(Qt::gray), -3);
	}
	preroll(m_defaultKey);

	if (m_mapPipelines.contains(m_backgroundKey))
	{
		m_mapPipelines.value(m_backgroundKey)->reconfigure(m_sdinfo);
	}
	else
	{
		m_backgroundKey = addStimulus(QString("background"), m_sdinfo.getBackGroundColor(), -2);
	}
	preroll(m_defaultKey);

	// just reconfigure ag if it already exists, don't create here
	if (m_mapPipelines.contains(m_agKey))
	{
		m_mapPipelines.value(m_agKey)->reconfigure(m_sdinfo);
		preroll(m_agKey);
	}

}

void HGMM::reset(HStimulusWidget *pCenter, const Habit::StimulusDisplayInfo& info, const QDir& dir)
{
	qDebug() << "HGMM::reset(HStimulusWidget *pCenter, info, dir)";
	reset(info, dir);
	setWidgets(pCenter);
}

void HGMM::reset(HStimulusWidget *pLeft, HStimulusWidget *pRight, const Habit::StimulusDisplayInfo& info, const QDir& dir)
{
	qDebug() << "HGMM::reset(HStimulusWidget *pLeft, HStimulusWidget *pRight, info, dir)";
	reset(info, dir);
	setWidgets(pLeft, pRight);
}

void HGMM::setWidgets(HStimulusWidget *p0, HStimulusWidget *p1)
{
	qWarning() << "HGMM::setWidgets - must check if static stim must be rebuilt";
	if (p1)
	{
		// have left and right
		m_pCenter = p0;
		m_pLeft = p0;
		m_pRight = p1;
		// DON'T DO THIS HERE!
		//m_pStimulusLayoutType = &HStimulusLayoutType::HStimulusLayoutLeftRight;
	}
	else
	{
		m_pCenter = p0;
		m_pLeft = m_pRight = NULL;
		//m_pStimulusLayoutType = &HStimulusLayoutType::HStimulusLayoutSingle;
	}
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

	// just call cleanup on all pipelines
	QMapIterator<unsigned int, HPipeline *> it(m_mapPipelines);
	while (it.hasNext())
	{
	    it.next();
	    it.value()->cleanup();
	}

	// cleanup static pipelines
	HStaticStimPipeline *p;
	p = dynamic_cast<HStaticStimPipeline *>(m_mapPipelines[m_backgroundKey]);
	if (!p)
	{
		qCritical() << "~HGMM - cannot cast background stim to static.";
	}
	else
	{
		p->forceCleanup();
	}
	p = dynamic_cast<HStaticStimPipeline *>(m_mapPipelines[m_defaultKey]);
	if (!p)
	{
		qCritical() << "~HGMM - cannot cast default stim to static.";
	}
	else
	{
		p->forceCleanup();
	}
	p = dynamic_cast<HStaticStimPipeline *>(m_mapPipelines[m_agKey]);
	if (!p)
	{
		qCritical() << "~HGMM - cannot cast AG stim to static.";
	}
	else
	{
		p->forceCleanup();
	}

	// exit main loop
	qDebug() << "quit main loop";
	g_main_loop_quit(m_pgml);
	qDebug() << "g_thread_join...";
	g_thread_join(m_gthread);
	qDebug() << "g_thread_join done\n";

	g_main_loop_unref(m_pgml);
	g_thread_unref(m_gthread);
	qDeleteAll(m_mapPipelines);
	m_mapPipelines.clear();
	qDebug() << "~HGMM() - done";
}

//void HGMM::setStimulusLayoutType(const HStimulusLayoutType& layoutType, HStimulusWidget *w0, HStimulusWidget *w1)
//{
//	m_pStimulusLayoutType = &layoutType;
//}

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
	QList<unsigned int> csl = m_mapContext.values(context);
	std::reverse(csl.begin(), csl.end());
	return csl;
}

void HGMM::addStimuli(const Habit::StimuliSettings& ss, int context, bool bForceSound)
{
	QListIterator<Habit::StimulusSettings> it(ss.stimuli());	// note: getting const HStimulusSettingsList&, it.next() will be const StimulusSettings&
	while (it.hasNext())
		addStimulus(it.next(), context, bForceSound);
	return;
}

unsigned int HGMM::addStimulus(unsigned int key, const Habit::StimulusSettings& stimulus, int context, bool bForceSound)
{
	HPipeline *pipeline;

	qDebug() << "HGMM::addStimulus(" << key << "): " << stimulus.getName() << " context " << context << " layout type " << getStimulusLayoutType().name();

	// create pipeline
	if (!bForceSound)
	{
		pipeline = m_pipelineFactory(key, stimulus, m_sdinfo, m_root, (context < 0), this);
	}
	else
	{
		// Modify a copy of StimulusDisplayInfo to ensure sound is used.
		Habit::StimulusDisplayInfo info(m_sdinfo);
		info.setUseISS(true);
		pipeline = m_pipelineFactory(key, stimulus, info, m_root, (context < 0), this);
	}

	// Add helper to map
	m_mapPipelines.insert(key, pipeline);

	// add key to context map
	m_mapContext.insert(context, key);

	return key;
}

unsigned int HGMM::addStimulus(unsigned int key, const QString& name, const QColor& color, int context, bool bForceSound)
{
	Habit::StimulusSettings ss(name, color);
	return addStimulus(key, ss, context, bForceSound);
//	HPipeline *pipeline;
//	Habit::StimulusSettings stimulus(name, color);
//
//	qDebug() << "HGMM::addStimulus(" << key << "): solid color stimulus - " << color << " context " << context;
//zzzzzzzzzz
//	pipeline = m_pipelineFactory(key, stimulus, m_sdinfo, m_root, (context < 0), this);
//
//	// Add helper to map
//	m_mapPipelines.insert(key, pipeline);
//
//	// add key to context map
//	m_mapContext.insert(context, key);
//
//	return key;
}

unsigned int HGMM::addStimulus(const Habit::StimulusSettings& ss, int context, bool bForceSound)
{
	return addStimulus(nextKey(), ss, context, bForceSound);
}

unsigned int HGMM::addStimulus(const QString& name, const QColor& color, int context, bool bForceSound)
{
	return addStimulus(nextKey(), name, color, context, bForceSound);
}

bool HGMM::getContext(unsigned int key, int& context)
{
	bool bFound = false;
	// iterate through all pairs in m_mapContext
	QMapIterator<int, unsigned int> i(m_mapContext);
	while (i.hasNext() && !bFound)
	{
		i.next();
		if (i.value() == key)
		{
		  bFound = true;
		  context = i.key();
		}
	}
	return bFound;
}

bool HGMM::replaceStimulus(unsigned int key, const Habit::StimulusSettings& stimulus, bool bForceSound)
{
	HPipeline *pipelineToBeReplaced = NULL;
	HPipeline *pipelineTheNewOne = NULL;
	int context;
	bool bOK = false;

	qDebug() << "HGMM::replaceStimulus(" << key << "): " << stimulus.getName();

	// first, make sure there is already a pipeline with that key.
	if (m_mapPipelines.contains(key))
	{
		pipelineToBeReplaced = m_mapPipelines.value(key);

		// Now get the context. If this fails, something is wrong, because every stim should have one.
		if (!getContext(key, context))
		{
			qCritical() << "Key is not found in context map!";
			Q_ASSERT(true);
		}
		else
		{
			// create pipeline. Use sdi from original -- it may have a modified sdi (if sound-only ag)
			Habit::StimulusDisplayInfo info(pipelineToBeReplaced->getStimulusDisplayInfo());
			info.setUseISS(bForceSound);
			pipelineTheNewOne = m_pipelineFactory(key, stimulus, info, m_root, (context < 0), this);

			// replace pipeline in map
			// don't add key to context map, its already there
			m_mapPipelines[key] = pipelineTheNewOne;

			// clean up and destroy old one
			if (key == m_backgroundKey || key == m_defaultKey || key == m_agKey)
			{
				HStaticStimPipeline *p;
				p = dynamic_cast<HStaticStimPipeline *>(pipelineToBeReplaced);
				if (p)
				{
					p->forceCleanup();
				}
			}
			else
			{
				pipelineToBeReplaced->cleanup();
			}
			delete pipelineToBeReplaced;

			bOK = true;
		}
	}
	else
	{
		qCritical() << "HGMM::initialize(): key " << key << " not found!";
	}

	return key;
}

unsigned int HGMM::addAG(const Habit::StimulusSettings& ssAG, bool bForceSound)
{
	m_agKey = addStimulus(ssAG, -1, bForceSound);
	preroll(m_agKey);
	return m_agKey;
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
	if (m_bPendingAG)
	{
		m_bRewindCurrentPipeline = true;
		qDebug() << "HGMM::nowPlaying: Q_EMIT(agStarted(" << m_iPendingStimKey << "))";
		Q_EMIT(agStarted(m_iPendingStimKey));
	}
	else if (m_bPendingStim)
	{
		qDebug() << "HGMM::nowPlaying: Q_EMIT(stimStarted(" << m_iPendingStimKey << "))";
		Q_EMIT(stimStarted(m_iPendingStimKey));
	}
	else
	{
		qCritical() << "HGMM::nowPlaying: No stim or ag pending!";
	}
}

void HGMM::stim(unsigned int key)
{
	qDebug() << "HGMM::stim(" << key << ")";
	m_bPendingStim = true;
	m_bPendingAG = false;
	m_iPendingStimKey = key;
	playStim(key);
}

void HGMM::initialize(unsigned int key)
{
	qDebug() << "HGMM::initialize(" << key << ") stim " << getStimulusSettings(key).getName();

	if (m_mapPipelines.contains(key))
	{
		m_mapPipelines.value(key)->initialize();
	}
	else
	{
		qWarning() << "HGMM::initialize(): key " << key << " not found!";
	}
}

void HGMM::cleanup(unsigned int key)
{
	qDebug() << "HGMM::cleanup(" << key << ") stim " << getStimulusSettings(key).getName();

	if (m_mapPipelines.contains(key))
	{
		m_mapPipelines.value(key)->cleanup();
	}
	else
	{
		qWarning() << "HGMM::cleanup(): key " << key << " not found!";
	}
}

// Don't get confused here - the input parameter is the stim key. Below, in the context multi map, the
// 'key' is the context, and the 'value' is the stim id (input parake.
void HGMM::remove(unsigned int stimkey)
{
	qDebug() << "HGMM::remove(" << stimkey << ") stim " << getStimulusSettings(stimkey).getName();

	if (m_mapPipelines.contains(stimkey))
	{
		m_mapPipelines.value(stimkey)->cleanup();
		HPipeline *p = m_mapPipelines.take(stimkey);
		if (p) delete p;

		// fix context list. Have to iterate over list to find the stim key.
		QMapIterator<int, unsigned int> it(m_mapContext);
		int context = -999;
		while (it.hasNext())
		{
			it.next();
			if (it.value() == stimkey)
			{
				context = it.key();
				break;
			}
		}
		Q_ASSERT(context != -999);
		m_mapContext.remove(context, stimkey);
	}
	else
	{
		qWarning() << "HGMM::remove(): stimkey " << stimkey << " not found!";
	}
}


void HGMM::preroll(unsigned int key)
{
	qDebug() << "HGMM::preroll(" << key << ") stim " << getStimulusSettings(key).getName();
	if (m_mapPipelines.contains(key))
	{
		m_mapPipelines.value(key)->preroll();
	}
	else
	{
		qWarning() << "HGMM::preroll(): key " << key << " not found!";
	}
}

void HGMM::pause(unsigned int key)
{
	qDebug() << "HGMM::pause(" << key << ") stim " << getStimulusSettings(key).getName();
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
	qDebug() << "HGMM::rewind(" << key << ") stim " << getStimulusSettings(key).getName();
	if (m_mapPipelines.contains(key))
	{
		m_mapPipelines.value(key)->rewind();
	}
	else
	{
		qWarning() << "HGMM::rewind(): key " << key << " not found!";
	}
}

void HGMM::dump(unsigned int key)
{
	qDebug() << "HGMM::dump(" << key << " stim " << getStimulusSettings(key).getName();

	if (m_mapPipelines.contains(key))
	{
		m_mapPipelines.value(key)->dump();
	}
	else
	{
		qWarning() << "HGMM::dump(): key " << key << " not found!";
	}
}


void HGMM::playStim(unsigned int key)
{
	qDebug() << "HGMM::playstim(" << key << ") stim " << getStimulusSettings(key).getName() << " layout " << getStimulusLayoutType().name();
	HPipeline *pipeline = NULL;		// the pipeline that will be played

	// get pipeline that will be displayed/played.
	// If the key is not found, that's bad. Use default.
	if (m_mapPipelines.contains(key))
	{
		pipeline = m_mapPipelines.value(key);
		pipeline->initialize();
	}
	else
	{
		qWarning() << "key " << key << " not found! Using default stim.";
		pipeline = m_mapPipelines.value(m_defaultKey);
	}

	if (pipeline != m_pipelineCurrent)
	{

		// deal with current pipeline. After this 'm_pipelineCurrent' is no longer connected to the display.
		// 	Its also been "cleaned", whatever that means.
		if (m_pipelineCurrent)
		{
			qDebug() << "HGMM::playstim: pause current stim";
			disconnect(m_pipelineCurrent, SIGNAL(nowPlaying()), this, SLOT(nowPlaying()));
			m_pipelineCurrent->pause();
			m_pipelineCurrent->detachWidgetsFromSinks();

			// check disposition of the current pipeline. By default it will be cleaned up, but that can
			// be overridden by calling rewindCurrentPipeline(). Such a call will only apply to m_pipelineCurrent,
			// and will revert to the default after this call is done.
			if (m_bRewindCurrentPipeline)
				m_pipelineCurrent->rewind();
			else
				m_pipelineCurrent->cleanup();	// might not cleanup
		}

		// set things up for new pipeline 'pipeline'
		connect(pipeline, SIGNAL(nowPlaying()), this, SLOT(nowPlaying()));
		if (getStimulusLayoutType()==HStimulusLayoutType::HStimulusLayoutSingle)
		{
			pipeline->attachWidgetsToSinks(m_pCenter->getHVideoWidget());
		}
		else if (getStimulusLayoutType()==HStimulusLayoutType::HStimulusLayoutLeftRight)
		{
			pipeline->attachWidgetsToSinks(m_pLeft->getHVideoWidget(), m_pRight->getHVideoWidget());
		}

	}
	m_bRewindCurrentPipeline = false;
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
	if (getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		m_pCenter->getHVideoWidget()->updateGeometry();
	}
	else if (getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		m_pLeft->getHVideoWidget()->updateGeometry();
		m_pRight->getHVideoWidget()->updateGeometry();
	}
}


const Habit::StimulusSettings& HGMM::getStimulusSettings(unsigned int key) const
{
	static Habit::StimulusSettings dummy;
	if (pipelineMap().contains(key))
		return pipelineMap().value(key)->stimulusSettings();
	else
		return dummy;
}

QDialog* HGMM::createStimulusWidget()
{
	QDialog *pDialog = new QDialog;
	QHBoxLayout *hbox = new QHBoxLayout;
	qDebug() << "createStimulusWidget - layout type " << getStimulusLayoutType().name();
	if (getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		qDebug() << "createStimulusWidget - single";
		HStimulusWidget *video = getHStimulusWidget(HPlayerPositionType::Center);
		connect(this, SIGNAL(stimulusChanged()), video->getHVideoWidget(), SLOT(stimulusChanged()));
		hbox->addWidget(video);
		pDialog->setLayout(hbox);
		pDialog->setMinimumSize(320, 240);
	}
	else if (getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		HStimulusWidget *videoLeft = getHStimulusWidget(HPlayerPositionType::Left);
		HStimulusWidget *videoRight = getHStimulusWidget(HPlayerPositionType::Right);
		connect(this, SIGNAL(stimulusChanged()), videoLeft->getHVideoWidget(), SLOT(stimulusChanged()));
		connect(this, SIGNAL(stimulusChanged()), videoRight->getHVideoWidget(), SLOT(stimulusChanged()));
		hbox->addWidget(videoLeft);
		hbox->addWidget(videoRight);
		pDialog->setLayout(hbox);
		qDebug() << "createStimulusWidget - L/R";
	}
	return pDialog;
}
