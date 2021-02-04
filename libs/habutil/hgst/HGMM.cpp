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
#include <QLabel>
#include <gst/gst.h>
#include <iostream>
#include <algorithm>





HGMM::HGMM(const Habit::StimulusDisplayInfo& sdi, QVector<HStimulusWidget *> vecWidgets, const QDir& stimRoot, const QString& name, PipelineFactory factory)
: m_sdinfo(sdi)
, m_root(stimRoot)
, m_pipelineCurrent(NULL)
, m_gthread(NULL)
, m_pgml(NULL)
, m_pipelineFactory(factory)
, m_name(name)
, m_backgroundKey(99998)
, m_agKey(99999)
, m_defaultKey(99997)
, m_bPendingAG(false)
, m_bPendingStim(false)
, m_iPendingStimKey(0)
{
	// launch main loop thread
	m_pgml = g_main_loop_new(NULL, FALSE);
	m_gthread = g_thread_new("HGMM-main-loop", &HGMM::threadFunc, m_pgml);

	// m_wid is a mapping of player position to window ID. See HStimPipeline::busSyncHandler
	if (vecWidgets.size() == 1)
	{
		m_widgets.insert(HPlayerPositionType::Center.number(), vecWidgets[0]);
	}
	else if (vecWidgets.size() == 2)
	{
		m_widgets.insert(HPlayerPositionType::Left.number(), vecWidgets[0]);
		m_widgets.insert(HPlayerPositionType::Right.number(), vecWidgets[1]);
	}
	else if (vecWidgets.size() == 3)
	{
		m_widgets.insert(HPlayerPositionType::Left.number(), vecWidgets[0]);
		m_widgets.insert(HPlayerPositionType::Center.number(), vecWidgets[1]);
		m_widgets.insert(HPlayerPositionType::Right.number(), vecWidgets[2]);
	}
	else
		qCritical() << "Cannot create HGMM with " << vecWidgets.size() << " widgets.";

	// default stim is background color, specified in the StimulusDisplayInfo
	addDefaultStim(sdi);

	// Now actually play/display the background. This means that instantiating this class
	// will display something in the widgets.
	background();
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


gpointer HGMM::threadFunc(gpointer user_data)
{
	GMainLoop *pgml = (GMainLoop *)user_data;
	// create and start main loop here
	qDebug() << "HGMM::threadFunc: starting main loop\n";
	g_main_loop_run(pgml);
	qDebug() << "HGMM::threadFunc: main loop ended\n";
	return NULL;
}

HGMM::~HGMM()
{
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
		qCritical() << "~HGMM: cannot cast background stim to static.";
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
	qDebug() << "HGMM(" << m_name << ")::~HGMM: quit main loop";
	g_main_loop_quit(m_pgml);
	qDebug() << "HGMM(" << m_name << ")::~HGMM: g_thread_join...";
	g_thread_join(m_gthread);
	qDebug() << "HGMM(" << m_name << ")::~HGMM: g_thread_join done\n";

	g_main_loop_unref(m_pgml);
	g_thread_unref(m_gthread);
	qDeleteAll(m_mapPipelines);
	m_mapPipelines.clear();
	qDebug() << "HGMM(" << m_name << ")::~HGMM() - done";
}


HStimulusWidget *HGMM::getHStimulusWidget(const HPlayerPositionType& type)
{
	if (m_widgets.contains(type.number()))
		return m_widgets[type.number()];
	return (HStimulusWidget *)NULL;
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

	qDebug() << "HGMM(" << m_name << ")::addStimulus(" << key << "): " << stimulus.getName() << " context " << context;

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

	// add signals
	connect(pipeline, SIGNAL(prerolling(int)), this, SIGNAL(prerolling(int)));
	connect(pipeline, SIGNAL(prerolled(int)), this, SIGNAL(prerolled(int)));

	// Add key to pipeline map
	m_mapPipelines.insert(key, pipeline);

	// add key to context map
	m_mapContext.insert(context, key);

	return key;
}


unsigned int HGMM::addDefaultStim(const Habit::StimulusDisplayInfo& sdi)
{
	m_backgroundKey = addStimulus("default", sdi.getBackGroundColor(), -2);
	qDebug() << "HGMM(" << m_name << ")::addDefaultStim " << sdi.getBackGroundColor() << " id " << m_backgroundKey;
	return m_backgroundKey;
}



unsigned int HGMM::addStimulus(unsigned int key, const QString& name, const QColor& color, int context, bool bForceSound)
{
	Habit::StimulusSettings ss(name, color);
	return addStimulus(key, ss, context, bForceSound);
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

	qDebug() << "HGMM(" << m_name << ")::replaceStimulus(" << key << "): " << stimulus.getName();

	// first, make sure there is already a pipeline with that key.
	if (m_mapPipelines.contains(key))
	{
		pipelineToBeReplaced = m_mapPipelines.value(key);

		// disconnect signals
		disconnect(pipelineToBeReplaced, SIGNAL(prerolling(int)), this, SIGNAL(prerolling(int)));
		disconnect(pipelineToBeReplaced, SIGNAL(prerolled(int)), this, SIGNAL(prerolled(int)));

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

			// signals
			connect(pipelineTheNewOne, SIGNAL(prerolling(int)), this, SIGNAL(prerolling(int)));
			connect(pipelineTheNewOne, SIGNAL(prerolled(int)), this, SIGNAL(prerolled(int)));


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
		qCritical() << "HGMM(" << m_name << ")::replaceStimulus(): key " << key << " not found!";
	}

	return key;
}

unsigned int HGMM::addAG(const Habit::StimulusSettings& ssAG, bool bForceSound)
{
	// AG is just a stimulus with context = -1
	// note we assume that there is only one ag ever - m_agKey is assigned here.
	m_agKey = addStimulus(ssAG, -1, bForceSound);

	// TODO - don't do this here!!!!
	preroll(m_agKey);

	return m_agKey;
}


void HGMM::ag()
{
	m_bPendingAG = true;
	m_bPendingStim = false;
	m_iPendingStimKey = m_agKey;
	playStim(m_agKey);
}

void HGMM::background()
{
	stim(m_backgroundKey);
}

void HGMM::nowPlaying()
{
	if (m_bPendingAG)
	{
		m_bRewindCurrentPipeline = true;
		qDebug() << "HGMM(" << m_name << ")::nowPlaying: Q_EMIT(agStarted(" << m_iPendingStimKey << "))";
		Q_EMIT(agStarted(m_iPendingStimKey));
	}
	else if (m_bPendingStim)
	{
		qDebug() << "HGMM(" << m_name << ")::nowPlaying: Q_EMIT(stimStarted(" << m_iPendingStimKey << "))";
		Q_EMIT(stimStarted(m_iPendingStimKey));
	}
	else
	{
		qCritical() << "HGMM(" << m_name << ")::nowPlaying: No stim or ag pending!";
	}
}

// replace current stimulus with the one identified by 'key'.
void HGMM::stim(unsigned int key)
{
	qDebug() << "HGMM(" << m_name << ")::stim(" << key << ")";
	m_bPendingStim = true;
	m_bPendingAG = false;
	m_iPendingStimKey = key;
	playStim(key);
}

void HGMM::initialize(unsigned int key)
{
	qDebug() << "HGMM(" << m_name << ")::initialize(" << key << ") stim " << getStimulusSettings(key).getName();

	if (m_mapPipelines.contains(key))
	{
		m_mapPipelines.value(key)->initialize();
	}
	else
	{
		qWarning() << "HGMM(" << m_name << ")::initialize(): key " << key << " not found!";
	}
}

void HGMM::cleanup(unsigned int key)
{
	qDebug() << "HGMM(" << m_name << ")::cleanup(" << key << ") stim " << getStimulusSettings(key).getName();

	if (m_mapPipelines.contains(key))
	{
		m_mapPipelines.value(key)->cleanup();
	}
	else
	{
		qWarning() << "HGMM(" << m_name << ")::cleanup(): key " << key << " not found!";
	}
}

// Don't get confused here - the input parameter is the stim key. Below, in the context multi map, the
// 'key' is the context, and the 'value' is the stim id (input parake.
void HGMM::remove(unsigned int stimkey)
{
	qDebug() << "HGMM(" << m_name << ")::remove(" << stimkey << ") stim " << getStimulusSettings(stimkey).getName();

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
		qWarning() << "HGMM(" << m_name << ")::remove(): stimkey " << stimkey << " not found!";
	}
}


void HGMM::preroll(unsigned int key)
{
	qDebug() << "HGMM(" << m_name << ")::preroll(" << key << ") stim " << getStimulusSettings(key).getName();
	if (m_mapPipelines.contains(key))
	{
		m_mapPipelines.value(key)->preroll();
	}
	else
	{
		qWarning() << "HGMM(" << m_name << ")::preroll(): key " << key << " not found!";
	}
}

void HGMM::pause(unsigned int key)
{
	qDebug() << "HGMM(" << m_name << ")::pause(" << key << ") stim " << getStimulusSettings(key).getName();
	if (m_mapPipelines.contains(key))
	{
		m_mapPipelines.value(key)->pause();
	}
	else
	{
		qWarning() << "HGMM(" << m_name << ")::pause(): key " << key << " not found!";
	}
}

void HGMM::rewind(unsigned int key)
{
	qDebug() << "HGMM(" << m_name << ")::rewind(" << key << ") stim " << getStimulusSettings(key).getName();
	if (m_mapPipelines.contains(key))
	{
		m_mapPipelines.value(key)->rewind();
	}
	else
	{
		qWarning() << "HGMM(" << m_name << ")::rewind(): key " << key << " not found!";
	}
}

void HGMM::dump(unsigned int key)
{
	qDebug() << "HGMM(" << m_name << ")::dump(" << key << " stim " << getStimulusSettings(key).getName();

	if (m_mapPipelines.contains(key))
	{
		m_mapPipelines.value(key)->dump();
	}
	else
	{
		qWarning() << "HGMM(" << m_name << ")::dump(): key " << key << " not found!";
	}
}


void HGMM::playStim(unsigned int key)
{
	qDebug() << "HGMM(" << m_name << ")::playstim(" << key << ") stim " << getStimulusSettings(key).getName();
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


	// detach widgets from sinks, if there is a current pipeline (which there should be
	// unless we've just started up).
	// Disconnect 'nowPlaying' signal from the pipeline. That signal is emitted when the
	// stimulus enters PLAYING state. Because of that, we pause and rewind if the current
	// pipeline is the same as the new one.

	if (pipeline != m_pipelineCurrent)
	{

		// deal with current pipeline. After this 'm_pipelineCurrent' is no longer connected to the display widget.
		if (m_pipelineCurrent)
		{
			qDebug() << "HGMM(" << m_name << ")::playstim: pause current stim";
			disconnect(m_pipelineCurrent, SIGNAL(nowPlaying()), this, SLOT(nowPlaying()));
			disconnect(m_pipelineCurrent, SIGNAL(screen(const QString&, int)), this, SIGNAL(screenStarted(const QString&, int)));
			m_pipelineCurrent->pause();


			qDebug() << "HGMM(" << m_name << ")::playstim: detach widgets from current stim";
			m_pipelineCurrent->detachWidgetsFromSinks();

			if (!m_pipelineCurrent->isStatic())
			{
				emit detached(m_pipelineCurrent->id());
			}

			// check disposition of the current pipeline. By default it will be cleaned up, but that can
			// be overridden by calling rewindCurrentPipeline(). Such a call will only apply to m_pipelineCurrent,
			// and will revert to the default after this call is done.

			if (m_bRewindCurrentPipeline)
				m_pipelineCurrent->rewind();
			else
				m_pipelineCurrent->cleanup();	// might not cleanup
		}

		connect(pipeline, SIGNAL(nowPlaying()), this, SLOT(nowPlaying()));
		connect(pipeline, SIGNAL(screen(const QString&, int)), this, SIGNAL(screenStarted(const QString&, int)));

//		// Attach new pipeline sinks to the widgets.
//		qDebug() << "HGMM::playstim: attach widgets to sinks";
//		if (getStimulusLayoutType()==HStimulusLayoutType::HStimulusLayoutSingle)
//		{
//			pipeline->attachWidgetsToSinks(m_pCenter->getHVideoWidget());
//		}
//		else if (getStimulusLayoutType()==HStimulusLayoutType::HStimulusLayoutLeftRight)
//		{
//			pipeline->attachWidgetsToSinks(m_pLeft->getHVideoWidget(), m_pRight->getHVideoWidget());
//		}
//		else if (getStimulusLayoutType()==HStimulusLayoutType::HStimulusLayoutTriple)
//		{
//			pipeline->attachWidgetsToSinks(m_pLeft->getHVideoWidget(), m_pCenter->getHVideoWidget(), m_pRight->getHVideoWidget());
//		}
//		else
//			qCritical() << "HGMM::playStim  stimulus layout type " << getStimulusLayoutType().name() << " not handled.";
	}
	else
	{
		// calling playStim on the stim that's currently playing. Handling has to be a little different.
		m_pipelineCurrent->pause();
		m_pipelineCurrent->rewind();
	}
	m_bRewindCurrentPipeline = false;
	qDebug() << "HGMM(" << m_name << ")::playstim: set to PLAY";
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
		disconnect(m_pipelineCurrent, SIGNAL(screen(const QString&, int)), this, SIGNAL(screenStarted(const QString&, int)));
		m_pipelineCurrent->pause();
		m_pipelineCurrent->detachWidgetsFromSinks();
	}
}

void HGMM::updateGeometry()
{
	QList<HStimulusWidget *> list = m_widgets.values();
	for (int i=0; i<list.size(); i++)
	{
		qDebug() << "HGMM(" << m_name << ")::updateGeometry" << i << "/" << list.size();
		list[i]->updateGeometry();
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

QWidget* HGMM::createStimulusWidget()
{
	QLabel *pLabel = new QLabel;
	//QDialog *pDialog = new QDialog;
	QHBoxLayout *hbox = new QHBoxLayout;
	if (m_sdinfo.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		qDebug() << "createStimulusWidget - single";
		HStimulusWidget *video = getHStimulusWidget(HPlayerPositionType::Center);
		connect(this, SIGNAL(stimulusChanged()), video->getHVideoWidget(), SLOT(stimulusChanged()));
		hbox->addWidget(video);
		pLabel->setLayout(hbox);
	}
	else if (m_sdinfo.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		HStimulusWidget *videoLeft = getHStimulusWidget(HPlayerPositionType::Left);
		HStimulusWidget *videoRight = getHStimulusWidget(HPlayerPositionType::Right);
		connect(this, SIGNAL(stimulusChanged()), videoLeft->getHVideoWidget(), SLOT(stimulusChanged()));
		connect(this, SIGNAL(stimulusChanged()), videoRight->getHVideoWidget(), SLOT(stimulusChanged()));
		hbox->addWidget(videoLeft);
		hbox->addWidget(videoRight);
		pLabel->setLayout(hbox);
		qDebug() << "createStimulusWidget - L/R";
	}
	else if (m_sdinfo.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutTriple)
	{
		HStimulusWidget *videoLeft = getHStimulusWidget(HPlayerPositionType::Left);
		HStimulusWidget *videoCenter = getHStimulusWidget(HPlayerPositionType::Center);
		HStimulusWidget *videoRight = getHStimulusWidget(HPlayerPositionType::Right);
		connect(this, SIGNAL(stimulusChanged()), videoLeft->getHVideoWidget(), SLOT(stimulusChanged()));
		connect(this, SIGNAL(stimulusChanged()), videoRight->getHVideoWidget(), SLOT(stimulusChanged()));
		connect(this, SIGNAL(stimulusChanged()), videoCenter->getHVideoWidget(), SLOT(stimulusChanged()));
		hbox->addWidget(videoLeft);
		hbox->addWidget(videoCenter);
		hbox->addWidget(videoRight);
		pLabel->setLayout(hbox);
		qDebug() << "createStimulusWidget - Triple";
	}
	return pLabel;
}
