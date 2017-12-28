/*
 * HGMM.h
 *
 *  Created on: Jul 11, 2017
 *      Author: dan
 */

#ifndef APPS_HG2_HGMM_H_
#define APPS_HG2_HGMM_H_

#include <QObject>
#include <QDir>
#include <QMultiMap>
#include <QTimer>
#include <QDialog>
#include <glib.h>
#include "HPipeline.h"
#include "HStimPipeline.h"
#include "HStaticStimPipeline.h"
#include "HTypes.h"
#include "stimulussettings.h"
#include "stimulisettings.h"
#include "HStimulusWidget.h"


typedef HPipeline* (*PipelineFactory)(int id, const Habit::StimulusSettings& stimulusSettings, const QDir& stimRoot, const HStimulusLayoutType& layoutType, bool bSound, bool bISS, bool bStatic, QObject *parent);

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
	HPipeline *m_pipelineCurrent;
	GThread *m_gthread;
	GMainLoop *m_pgml;
	bool m_bReady;
	PipelineFactory m_pipelineFactory;

	QMap<unsigned int, Habit::StimulusSettings> m_mapStimulusSettings;

	// A map containing lists of stimulus keys (which can be played with stim(key), and whose
	// StimulusSettings can be fetched with getStimulusSettings(key)), saved according to their
	// context. The context is a logical grouping of stimuli. Each phase has its own context, as does
	// the "attention getters", which has a fixed context.
	//
	// Note about context - context < 0 treated as "static" -- are not cleaned up. Have to remember which these are and
	// call forceCleanup() on each in d'tor.
	//
	// getContextStimList fetches a list of keys for the stimuli keys added for a given context.
	QMultiMap<int, unsigned int> m_mapContext;
	unsigned int m_backgroundKey;
	unsigned int m_agKey;
	unsigned int m_defaultKey;	// this key will always work

	void playStim(unsigned int key);
	static gpointer threadFunc(gpointer user_data);
	void updateGeometry();

	// Get the next key for m_mapStim. The values are doled out sequentially. Nothing special,
	// this is a convenience.
	unsigned int nextKey();
	unsigned int addStimulus(unsigned int key, const Habit::StimulusSettings& ss, int context);
	unsigned int addStimulus(unsigned int key, const QString& name, const QColor& color, int context);

	bool m_bPendingAG;
	bool m_bPendingStim;
	unsigned int m_iPendingStimKey;

protected:

	// A map containing StimulusSettings objects, key is an integer.
	QMap<unsigned int, HPipeline *> m_mapPipelines;

public:

	HGMM(HStimulusWidget *center, const QDir& dir = QDir("/"), bool useISS = true, const QColor& bkgdColor = Qt::gray, PipelineFactory factory = HStimPipelineFactory);
	HGMM(HStimulusWidget *left, HStimulusWidget *right, const QDir& dir = QDir("/"), bool useISS = true, const QColor& bkgdColor = Qt::gray, PipelineFactory factory = HStimPipelineFactory);
	virtual ~HGMM();


	virtual unsigned int addAG(const Habit::StimulusSettings& ags);
	virtual void addStimuli(const Habit::StimuliSettings& ss, int context);
	virtual unsigned int addStimulus(const Habit::StimulusSettings& ss, int context);
	virtual unsigned int addStimulus(const QString& name, const QColor& color, int context);


	HStimulusWidget *getHStimulusWidget(const HPlayerPositionType& type);
	const HStimulusLayoutType& getStimulusLayoutType() const { return m_stimulusLayoutType; };
	const Habit::StimulusSettings getStimulusSettings(unsigned int key) const;
	QList<unsigned int> getContextStimList(int context);

	QDialog* createStimulusWidget();

	// set all pipelines to paused state, then monitor and wait for all pads to be assigned.
	// emit mmReady() if all are ready; emit mmFail() if not ready before timeout ms.
	// DO NOT USE void getReady(int ms);

	// currently a no-op, that may change djs 11-1-17
	bool waitForStimuliReady(int maxMS, int checkInterval = 200);

	// each pipeline represents a single stimulus (single or dual screen, i.e. Habit::StimulusSettings)
	const QMap<unsigned int, HPipeline *>& pipelineMap() { return m_mapPipelines; };
	unsigned int nStimuli() const { return m_mapPipelines.size(); };

	// pipeline control
	void initialize(unsigned int id);
	void preroll(unsigned int id);
	void cleanup(unsigned int id);
	void pause(unsigned int id);
	void rewind(unsigned int id);

	// pipeline info dump
	void dump(unsigned int id);

	virtual void defaultStim() { stim(m_defaultKey); };
	virtual void background() { stim(m_backgroundKey); };
	virtual void clear();
	virtual void stop();

public Q_SLOTS:

	void stim(unsigned int);
	void ag();
//	void readyCheck();
//	void readyFail();
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
