/*
 * HGMM.h
 *
 *  Created on: Jul 11, 2017
 *      Author: dan
 */

#ifndef APPS_HG2_HGMM_H_
#define APPS_HG2_HGMM_H_

#include <hgst/HStimulusWidget.h>

#include <QObject>
#include <QDir>
#include <QMultiMap>
#include <QTimer>
#include <QGst/Ui/VideoWidget>
#include <QGst/Element>
#include <QGst/Pipeline>
#include <QGst/Pad>
#include "HGMMHelper.h"
#include "HTypes.h"
#include "stimulussettings.h"
#include "stimulisettings.h"


// Media manager for Habit.
// This media manager is entirely new for Habit 2.2+. It uses GStreamer, a media library available
// on all platforms (mac, windows, linux).
// The HGMM object is constructed with video widgets (HVideoWidget to be precise). The widgets are needed
// by this class (and its associated class, HGMMHelper) to direct media streams to the screen(s) and
// audio outputs.

class HGMM: public QObject
{
	Q_OBJECT

	const HStimulusLayoutType& m_stimulusLayoutType;
	QDir m_root;
	bool m_bUseISS;
	HStimulusWidget *m_pCenter, *m_pLeft, *m_pRight;
	QTimer *m_readyTimeout, *m_readyCheck;
	HGMMHelper *m_pHelperCurrent;

	// A map containing lists of stimulus keys (which can be played with stim(key), and whose
	// StimulusSettings can be fetched with getStimulusSettings(key)), saved according to their
	// context. The context is a logical grouping of stimuli. Each phase has its own context, as does
	// the "attention getters", which has a fixed context.
	//
	// getContextStimList fetches a list of keys for the stimuli added for a given context.
	QMultiMap<int, unsigned int> m_mapContext;
	unsigned int m_backgroundKey;
	unsigned int m_agKey;
	unsigned int m_defaultKey;	// this key will always work

	void playStim(unsigned int key);

	// Get the next key for m_mapStim. The values are doled out sequentially. Nothing special,
	// this is a convenience.
	unsigned int nextKey();
	unsigned int addStimulus(unsigned int key, const Habit::StimulusSettings& ss, int context);
	unsigned int addStimulus(unsigned int key, const QString& name, const QColor& color, int context);
	//void addToPipeline(HGMMData *pdata, const Habit::StimulusInfo& info);

	bool m_bPendingAG;
	bool m_bPendingStim;
	unsigned int m_iPendingStimKey;

protected:

	// A map containing StimulusSettings objects, key is an integer.
	QMap<unsigned int, HGMMHelper *> m_mapData;

public:
	HGMM(HStimulusWidget *center, const QDir& dir = QDir("/"), bool useISS = true, const QColor& bkgdColor = Qt::gray);
	HGMM(HStimulusWidget *left, HStimulusWidget *right, const QDir& dir = QDir("/"), bool useISS = true, const QColor& bkgdColor = Qt::gray);
	virtual ~HGMM();


	virtual void clear();
	virtual unsigned int addAG(const Habit::StimulusSettings& ags);
	virtual unsigned int addBackground(const QColor& color);
	virtual void addStimuli(const Habit::StimuliSettings& ss, int context);
	virtual unsigned int addStimulus(const Habit::StimulusSettings& ss, int context);
	virtual unsigned int addStimulus(const QString& name, const QColor& color, int context);

	//QGst::Ui::VideoWidget *getVideoWidget(const HPlayerPositionType& type);
	HStimulusWidget *getHStimulusWidget(const HPlayerPositionType& type);
	const HStimulusLayoutType& getStimulusLayoutType() const { return m_stimulusLayoutType; };
	const Habit::StimulusSettings& getStimulusSettings(int key) const;
	QList<unsigned int> getContextStimList(int context);

	// check if all pipelines are prerolled (in paused state and async done); this means any can be started and it will start up asap.
	//bool isReady() const;

	// set all pipelines to paused state, then monitor and wait for all pads to be assigned.
	// emit mmReady() if all are ready; emit mmFail() if not ready before timeout ms.
	void getReady(int ms);

	//const QMap<unsigned int, const Habit::StimulusSettings *>& pmap() { return m_mapPStimulusSettings; };
	const QMap<unsigned int, HGMMHelper *>& dataMap() { return m_mapData; };
public Q_SLOTS:

	void stim(unsigned int);
	void ag();
	void readyCheck();
	void readyFail();
	void nowPlaying();
//	void playerStarted(int i, const QString& filename);
//	void playerCleared(int i);

Q_SIGNALS:
	void agStarted(int);
	void stimStarted(int);
	void cleared();
	void screen(int, const QString&);
	void mmReady();
	void mmFail();
	void stimulusChanged();

};
#endif /* APPS_HG2_HGMM_H_ */