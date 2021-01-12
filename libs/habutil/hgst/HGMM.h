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
#include <QVector>
#include <QTimer>
#include <QDialog>
#include <glib.h>
#include "HPipeline.h"
#include "HStimPipeline.h"
#include "HStaticStimPipeline.h"
#include "HTypes.h"
#include "experimentsettings.h"
#include "stimulussettings.h"
#include "stimulisettings.h"
#include "HStimulusWidget.h"


typedef HPipeline* (*PipelineFactory)(int id, const Habit::StimulusSettings& stimulusSettings, const Habit::StimulusDisplayInfo& info, const PPTWIdMap& pwMap, const QDir& stimRoot, bool bStatic, QObject *parent);

// Media manager for Habit.
// This media manager is entirely new for Habit 2.2+. It uses GStreamer, a media library available
// on all platforms (mac, windows, linux).
// The HGMM object is constructed with video widgets (HVideoWidget to be precise). The widgets are needed
// by this class (and its associated class, HGMMHelper) to direct media streams to the screen(s) and
// audio outputs.

class HGMM: public QObject
{
	Q_OBJECT

	Habit::StimulusDisplayInfo m_sdinfo;
	QDir m_root;
	//bool m_bUseISS;
	//HStimulusWidget *m_pCenter, *m_pLeft, *m_pRight;
	// map of WId from the widgets above
	QMap<int, HStimulusWidget *> m_widgets;
	PPTWIdMap m_wid;
	HPipeline *m_pipelineCurrent;
	GThread *m_gthread;
	GMainLoop *m_pgml;
	PipelineFactory m_pipelineFactory;
	QString m_name;

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

	HGMM(HGMM& mm);	// not defined.


	void playStim(unsigned int key);

	// Thread func that runs the GMainLoop and exits.
	static gpointer threadFunc(gpointer user_data);

	// Call updateGeometry() on each non-NULL HStimulusWidget.
	void updateGeometry();

	// cleanup all pipelines, including static.
	void cleanupAll();

	// Get the next key for m_mapStim. The values are doled out sequentially. Nothing special,
	// this is a convenience.
	unsigned int nextKey();

	// add stimulus using key 'key', and the stimulus settings 'ss'. Applies to the context/phase 'context'.
	// If 'bForceSound' is true, the pipeline will generate sound even if the StimulusDisplayInfo says otherwise
	unsigned int addStimulus(unsigned int key, const Habit::StimulusSettings& ss, int context, bool bForceSound = false);
	unsigned int addStimulus(unsigned int key, const QString& name, const QColor& color, int context, bool bForceSound = false);
	unsigned int addDefaultStim(const Habit::StimulusDisplayInfo& sdi);
	bool getContext(unsigned int key, int& context);
	bool m_bPendingAG;
	bool m_bPendingStim;
	unsigned int m_iPendingStimKey;
	bool m_bRewindCurrentPipeline;

protected:

	// A map containing StimulusSettings objects, key is an integer.
	QMap<unsigned int, HPipeline *> m_mapPipelines;

public:

	// Different ctor for each of the config types as defined by sdi. Must match, sorry.
//	HGMM(const Habit::StimulusDisplayInfo& sdi, HStimulusWidget *pCenter, PipelineFactory factory = HStimPipelineFactory);
//	HGMM(const Habit::StimulusDisplayInfo& sdi, HStimulusWidget *pLeft, HStimulusWidget *pRight, PipelineFactory factory = HStimPipelineFactory);
//	HGMM(const Habit::StimulusDisplayInfo& sdi, HStimulusWidget *pLeft, HStimulusWidget *pCenter, HStimulusWidget *pRight, PipelineFactory factory = HStimPipelineFactory);
	HGMM(const Habit::StimulusDisplayInfo& sdi, QVector<HStimulusWidget *> vecWidgets, const QDir& stimRoot, const QString& name = QString("unnamed"), PipelineFactory factory = HStimPipelineFactory);

	virtual ~HGMM();

	// Add attention getter stimulus. This stimulus is then prerolled and made ready to go.
	// Returns stimulus ID.
	virtual unsigned int addAG(const Habit::StimulusSettings& ags, bool bForceSound = false);

	// Add individual solid-color stimulus.
	virtual unsigned int addStimulus(const Habit::StimulusSettings& ss, int context, bool bForceSound = false);

	// Add stimulus specified in StimulusSettings. Return value is stimID.
	virtual unsigned int addStimulus(const QString& name, const QColor& color, int context, bool bForceSound = false);

	// update the given pipeline to use a new stim. Used when changing AG stim, e.g.
	virtual bool replaceStimulus(unsigned int key, const Habit::StimulusSettings& stimulus, bool bForceSound = false);

	// add all stimuli in StimuliSettings. StimuliSettings intended to hold all
	// stim for a single context (phase). To get the stimIDs, call getContextStimList.
	// StimIDs for the stimuli are in same order as StimulusSettings are in the StimuliSettings.
	virtual void addStimuli(const Habit::StimuliSettings& ss, int context, bool bForceSound = false);
	QList<unsigned int> getContextStimList(int context);

	// Convenience funcs for standard stimIDs
	unsigned int getAGKey() const { return m_agKey; };
	unsigned int getBackgroundKey() const { return m_backgroundKey; };
	unsigned int getDefaultKey() const { return m_defaultKey; };

	// Get the widget for a particular position
	HStimulusWidget *getHStimulusWidget(const HPlayerPositionType& type);


	const Habit::StimulusSettings& getStimulusSettings(unsigned int key) const;


	QWidget *createStimulusWidget();

	// each pipeline represents a single stimulus (single or dual screen, i.e. Habit::StimulusSettings)
	const QMap<unsigned int, HPipeline *>& pipelineMap() const { return m_mapPipelines; };

	// tell hgmm that currently playing stim should be rewound, not cleaned up
	void rewindCurrentPipeline() { m_bRewindCurrentPipeline = true; };

	// pipeline control
	void initialize(unsigned int id);

	// remove stimulus, delete pipeline, make lists consistent
	void remove(unsigned int id);

	// pipeline info dump
	void dump(unsigned int id);

	virtual void defaultStim() { stim(m_defaultKey); };
	virtual void stop();

public Q_SLOTS:

	// replace current stimulus with the one id'd by the key
	void stim(unsigned int);
	void preroll(unsigned int id);
	void cleanup(unsigned int id);
	void pause(unsigned int id);
	void rewind(unsigned int id);
	void background();

	// replace current stimulus with the one id'd by the current ag key
	void ag();

	// when stim is playing - as denoted by gstreamer's bus message
	// indicating pipeline has changed state from GST_STATE_PAUSED to
	// GST_STATE_PLAYING
	void nowPlaying();

Q_SIGNALS:
	void agStarted(int);
	void stimStarted(int);
	void screenStarted(const QString&, int);
	//void screen(int, const QString&);
	void stimulusChanged();
	void prerolling(int);
	void prerolled(int);
	void detached(int);

};
#endif /* APPS_HG2_HGMM_H_ */
