/*
 * HGMM.cpp
 *
 *  Created on: Jul 11, 2017
 *      Author: dan
 */

#include "HGMM.h"
#include "HGMMHelper.h"
#include <QGlib/Connect>
#include <QGst/ElementFactory>
#include <QMapIterator>
#include <QEventLoop>
#include <gst/gst.h>


#define SPECIALPLAYHACK 1

//const Habit::StimulusSettings HGMM::dummyStimulusSettings;

HGMM::HGMM(HStimulusWidget *center, const QDir& dir, bool useISS, const QColor& bkgdColor)
: m_stimulusLayoutType(HStimulusLayoutType::HStimulusLayoutSingle)
, m_root(dir)
, m_bUseISS(useISS)
, m_pCenter(center)
, m_pLeft(NULL)
, m_pRight(NULL)
, m_pHelperCurrent(NULL)
{
	m_readyTimeout = new QTimer(this);
	m_readyTimeout->setSingleShot(true);
	m_readyCheck = new QTimer(this);
	connect(m_readyTimeout, SIGNAL(timeout()), this, SLOT	(readyFail()));
	connect(m_readyCheck, SIGNAL(timeout()), this, SLOT(readyCheck()));
	m_defaultKey = addStimulus(QString("default"), QColor(Qt::gray), -3);
	addBackground(bkgdColor);
}

HGMM::HGMM(HStimulusWidget *left, HStimulusWidget *right, const QDir& dir, bool useISS, const QColor& bkgdColor)
: m_stimulusLayoutType(HStimulusLayoutType::HStimulusLayoutLeftRight)
, m_root(dir)
, m_bUseISS(useISS)
, m_pCenter(NULL)
, m_pLeft(left)
, m_pRight(right)
, m_pHelperCurrent(NULL)
{
	m_readyTimeout = new QTimer(this);
	m_readyTimeout->setSingleShot(true);
	m_readyCheck = new QTimer(this);
	connect(m_readyTimeout, SIGNAL(timeout()), this, SIGNAL(readyFail()));
	connect(m_readyCheck, SIGNAL(timeout()), this, SLOT(readyCheck()));
	m_defaultKey = addStimulus(QString("default"), QColor(Qt::gray), -3);
	addBackground(bkgdColor);
}

HGMM::~HGMM()
{
	// set all pipelines to PAUSED
	QMapIterator<unsigned int, HGMMHelper *> it(m_mapData);
	while (it.hasNext())
	{
	    it.next();
	    it.value()->pipeline()->setState(QGst::StateNull);
	}
}

#if 0
QGst::Ui::VideoWidget *HGMM::getVideoWidget(const HPlayerPositionType& type)
{
	HStimulusWidget *w = getHVideoWidget(type);
	QGst::Ui::VideoWidget *vw = NULL;
	if (w)
		vw = w->getVideoWidget();
	return vw;
}
#endif


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
	return m_mapData.size();
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
	HGMMHelper *pHelper;

	qDebug() << "Adding stimulus (key " << key << "): " << stimulus << " context " << context;

	// Create a helper.
	if (m_stimulusLayoutType == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		pHelper = new HGMMHelper(key, stimulus, m_root, m_pCenter->getHVideoWidget(), m_bUseISS, this);

	}
	else if (m_stimulusLayoutType == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		pHelper = new HGMMHelper(key, stimulus, m_root, m_pLeft->getHVideoWidget(), m_pRight->getHVideoWidget(), m_bUseISS, this);
	}

	// Add helper to map
	m_mapData.insert(key, pHelper);

	// add key to context map
	m_mapContext.insert(context, key);

	return key;
}

unsigned int HGMM::addStimulus(unsigned int key, const QString& name, const QColor& color, int context)
{
	HGMMHelper *pHelper;
	Habit::StimulusSettings stimulus(name, color);

	qDebug() << "Adding solid color stimulus (key " << key << "): " << color << " context " << context;

	// Create a helper.
	if (m_stimulusLayoutType == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		pHelper = new HGMMHelper(key, stimulus, m_root, m_pCenter->getHVideoWidget(), m_bUseISS, this);
	}
	else if (m_stimulusLayoutType == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		pHelper = new HGMMHelper(key, stimulus, m_root, m_pLeft->getHVideoWidget(), m_pRight->getHVideoWidget(), m_bUseISS, this);
	}

	// Add helper to map
	m_mapData.insert(key, pHelper);

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
	return (m_agKey = addStimulus(ssAG, -1));
}

unsigned int HGMM::addBackground(const QColor& color)
{
	qWarning() << "TODO: ensure addBackground is called only once";
	return (m_backgroundKey = addStimulus(QString("background"), color, -2));
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
		Q_EMIT(agStarted(m_iPendingStimKey));
	}
	else if (m_bPendingStim)
	{
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

void HGMM::playStim(unsigned int key)
{
	//qDebug() << "playStim " << key;
	HGMMHelper *phelper = NULL;
	if (m_mapData.contains(key))
	{
		phelper = m_mapData.value(key);
	}
	else
	{
		qWarning() << "key " << key << " not found! Using default stim.";
		phelper = m_mapData.value(m_defaultKey);
	}

	// if current stim is same as new stim, return
	if (phelper != m_pHelperCurrent)
	{
		// pause current stim
		if (m_pHelperCurrent)
		{
			m_pHelperCurrent->pipeline()->setState(QGst::StatePaused);
		}
		else
		{
			qDebug() << "no current stim";
		}


		// set up signals
		if (m_pHelperCurrent)
		{
			disconnect(m_pHelperCurrent, SIGNAL(nowPlaying()), this, SLOT(nowPlaying()));
		}
		// assign new stim sink to widget(s)
		connect(phelper, SIGNAL(nowPlaying()), this, SLOT(nowPlaying()));
		if (m_stimulusLayoutType == HStimulusLayoutType::HStimulusLayoutSingle)
		{
			HVideoWidget *pc = m_pCenter->getHVideoWidget();
			pc->releaseVideoSink();
			pc->setVideoSink(phelper->sink(HPlayerPositionType::Center));

			// let everybody know we just switched to a new sink
			Q_EMIT stimulusChanged();

			// now set the pipeline in motion
			phelper->play();
			m_pHelperCurrent = phelper;
		}
		else if (m_stimulusLayoutType == HStimulusLayoutType::HStimulusLayoutLeftRight)
		{
			m_pLeft->getHVideoWidget()->releaseVideoSink();
			m_pLeft->getHVideoWidget()->setVideoSink(phelper->sink(HPlayerPositionType::Left));
			m_pRight->getHVideoWidget()->releaseVideoSink();
			m_pRight->getHVideoWidget()->setVideoSink(phelper->sink(HPlayerPositionType::Right));

			Q_EMIT stimulusChanged();

			phelper->play();
			m_pHelperCurrent = phelper;
		}
	}
	else
	{
		qWarning() << "Calling stim() with currently playing stimulus.";
	}
}

bool HGMM::getReady(int ms)
{
	m_readyTimeout->start(ms);
	m_readyCheck->start(500);

	QEventLoop loop;
	connect(m_readyCheck,  SIGNAL(timeout()), this, SLOT(readyCheck()) );
	connect(m_readyTimeout, SIGNAL(timeout()), &loop, SLOT(quit()));
	m_readyTimeout->start(ms);
	m_readyCheck->start(500);
	loop.exec();

	return isReady();
#if 0
	// start a timer for the longest we'll wait for things to be ready
	m_readyTimeout->start(ms);
	m_readyCheck->start(500);

	// set all pipelines to PAUSED
	QMapIterator<unsigned int, HGMMHelper *> it(m_mapData);
	while (it.hasNext())
	{
	    it.next();
	    it.value()->pipeline()->setState(QGst::StatePaused);
	}
#endif
}



#if 0
QTimer timer;
timer.setSingleShot(true);
QEventLoop loop;
connect(sslSocket,  SIGNAL(encrypted()), &loop, SLOT(quit()) );
connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
timer.start(msTimeout);
loop.exec();

if(timer.isActive())
    qDebug("encrypted");
else
    qDebug("timeout");

#endif

// TODO: need locking???

bool HGMM::isReady() const
{
	bool bReady = true;
	QMapIterator<unsigned int, HGMMHelper *> it(m_mapData);
	while (bReady && it.hasNext())
	{
	    it.next();
	    if (!it.value()->hasPads()) bReady = false;
	}
	return bReady;
}

void HGMM::readyCheck()
{
	bool bReady = true;
	qDebug() << "readyCheck():";
	// iterate through helpers
	QMapIterator<unsigned int, HGMMHelper *> it(m_mapData);
	while (it.hasNext())
	{
	    it.next();
	    qDebug() << it.key() << ": has pads? " << it.value()->hasPads() << " state " << HGMMHelper::stateName(it.value()->pipeline()->currentState());
	    if (it.value()->pipeline()->currentState() != QGst::StatePaused && it.value()->pipeline()->currentState() != QGst::StatePlaying) bReady = false;
	}
	if (bReady)
	{
		qDebug() << "Ready";
		m_readyCheck->stop();
		m_readyTimeout->stop();
		Q_EMIT mmReady();

		// start display here
		stim(m_defaultKey);
	}
	else
		qDebug() << "Not Ready";
}

void HGMM::readyFail()
{
	// kill check timer
	m_readyCheck->stop();

	// emit failure signal
	Q_EMIT mmFail();
}

const Habit::StimulusSettings& HGMM::getStimulusSettings(int key) const
{
	static Habit::StimulusSettings dummy;
	if (m_mapData.contains(key))
		return m_mapData[key]->stimulusSettings();
	else
		return dummy;
}
