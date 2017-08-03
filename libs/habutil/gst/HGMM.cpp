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


#define SPECIALPLAYHACK 1

//const Habit::StimulusSettings HGMM::dummyStimulusSettings;

HGMM::HGMM(QGst::Ui::VideoWidget *center, const QDir& dir, bool useISS)
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

}

HGMM::HGMM(QGst::Ui::VideoWidget *left, QGst::Ui::VideoWidget *right, const QDir& dir, bool useISS)
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
}

HGMM::~HGMM() {
	// TODO Auto-generated destructor stub
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

	qDebug() << "Adding stimulus (key " << key << "): " << stimulus;

	// Create a helper.
	if (m_stimulusLayoutType == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		pHelper = new HGMMHelper(key, stimulus, m_root, m_pCenter, m_bUseISS, this);

	}
	else if (m_stimulusLayoutType == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		pHelper = new HGMMHelper(key, stimulus, m_root, m_pLeft, m_pRight, m_bUseISS, this);
	}

	// Add helper to map
	m_mapData.insert(key, pHelper);
	return key;
}

unsigned int HGMM::addStimulus(unsigned int key, const QString& name, const QColor& color, int context)
{
	HGMMHelper *pHelper;
	Habit::StimulusSettings stimulus(name, color);

	qDebug() << "Adding solid color stimulus (key " << key << "): " << color;

	// Create a helper.
	if (m_stimulusLayoutType == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		pHelper = new HGMMHelper(key, stimulus, m_root, m_pCenter, m_bUseISS, this);

	}
	else if (m_stimulusLayoutType == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		pHelper = new HGMMHelper(key, stimulus, m_root, m_pLeft, m_pRight, m_bUseISS, this);
	}

	// Add helper to map
	m_mapData.insert(key, pHelper);
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
	m_bPendingStim = true;
	m_bPendingAG = false;
	m_iPendingStimKey = key;
	playStim(key);
}

void HGMM::playStim(unsigned int key)
{
	qDebug() << "playStim " << key;
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
			m_pCenter->releaseVideoSink();
			m_pCenter->setVideoSink(phelper->sinkCenter());


			phelper->play();
			m_pHelperCurrent = phelper;
		}
		else if (m_stimulusLayoutType == HStimulusLayoutType::HStimulusLayoutLeftRight)
		{
			m_pLeft->releaseVideoSink();
			m_pLeft->setVideoSink(phelper->sinkLeft());
			m_pRight->releaseVideoSink();
			m_pRight->setVideoSink(phelper->sinkRight());
			phelper->play();
			m_pHelperCurrent = phelper;
		}
	}
	else
	{
		qWarning() << "Calling stim() with currently playing stimulus.";
	}
}

void HGMM::getReady(int ms)
{
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
	    qDebug() << it.key() << ": has pads? " << it.value()->hasPads();
	    if (!it.value()->hasPads()) bReady = false;
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

#if 0
void HGMMCallback::padAdded(const QGlib::ObjectPtr & sender, const QGst::PadPtr & srcPad)
{
	qDebug() << "HGMMCallback::padAdded(" << m_name << "), sender " << sender->property("name").get<QString>() << " new pad: " << srcPad->name();
}
#endif
