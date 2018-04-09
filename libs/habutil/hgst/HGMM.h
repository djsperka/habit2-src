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
#include "experimentsettings.h"
#include "stimulussettings.h"
#include "stimulisettings.h"
#include "HStimulusWidget.h"


typedef HPipeline* (*PipelineFactory)(int id, const Habit::StimulusSettings& stimulusSettings, const Habit::StimulusDisplayInfo& info, const QDir& stimRoot, bool bStatic, QObject *parent);

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
	HStimulusWidget *m_pCenter, *m_pLeft, *m_pRight;
	HPipeline *m_pipelineCurrent;
	GThread *m_gthread;
	GMainLoop *m_pgml;
	PipelineFactory m_pipelineFactory;

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


	HGMM(PipelineFactory factory = HStimPipelineFactory);
	HGMM(HGMM& mm);	// not defined.
	void playStim(unsigned int key);
	static gpointer threadFunc(gpointer user_data);
	void updateGeometry();

	// cleanup all pipelines, including static.
	void cleanupAll();

	// Get the next key for m_mapStim. The values are doled out sequentially. Nothing special,
	// this is a convenience.
	unsigned int nextKey();
	unsigned int addStimulus(unsigned int key, const Habit::StimulusSettings& ss, int context, bool bForceSound = false);
	unsigned int addStimulus(unsigned int key, const QString& name, const QColor& color, int context, bool bForceSound = false);
	bool getContext(unsigned int key, int& context);
	bool m_bPendingAG;
	bool m_bPendingStim;
	unsigned int m_iPendingStimKey;

protected:

	// A map containing StimulusSettings objects, key is an integer.
	QMap<unsigned int, HPipeline *> m_mapPipelines;

public:

//	HGMM(HStimulusWidget *center, const QDir& dir = QDir("/"), bool useISS = true, const QColor& bkgdColor = Qt::gray, PipelineFactory factory = HStimPipelineFactory);
//	HGMM(HStimulusWidget *left, HStimulusWidget *right, const QDir& dir = QDir("/"), bool useISS = true, const QColor& bkgdColor = Qt::gray, PipelineFactory factory = HStimPipelineFactory);
	virtual ~HGMM();

	// Perform a FULL RESET: clear everything and restore HGMM to its initial state, with no pipelines, not even static ones.
	void reset();

	// Re-initializes the mm, creates pipelines for default, background, and attention-getter (if configured),
	// and all configured stimuli. The default, background, and attention-getter are prerolled.
	// No widgets are set - call setWidgets() for that. Otherwise,
	// the mm will be ready to play all stimuli. Context stim lists are also available.
	void reset(const Habit::ExperimentSettings& settings, const QDir& dir = QDir::rootPath());

	// Reconfigure all existing pipelines using the supplied StimulusDisplayInfo. The stimuli themselves remain, but they get
	// all-new pipelines reconfigured using the new Info.
	// Intended for use when in ExperimentEditor - this must be called when no widgets are
	// displayed or even playing (not really sure what happens, but its easy to ensure so this is cautionary).
	void reset(const Habit::StimulusDisplayInfo& info, const QDir& dir = QDir::rootPath());


	// Re-initialize mm, create pipelines for default, background. Widgets ARE set, but no other stimuli or ag
	// are configured.
	void reset(HStimulusWidget *pCenter, const Habit::StimulusDisplayInfo& info, const QDir& dir = QDir::rootPath());
	void reset(HStimulusWidget *pLeft, HStimulusWidget *pRight,  const Habit::StimulusDisplayInfo& info, const QDir& dir = QDir::rootPath());

	// assign widget(s) to hgmm
	void setWidgets(HStimulusWidget *p0, HStimulusWidget *p1=NULL);

	// Get reference to singleton instance of HGMM.
	static HGMM& instance();

	virtual unsigned int addAG(const Habit::StimulusSettings& ags, bool bForceSound = false);
	virtual void addStimuli(const Habit::StimuliSettings& ss, int context, bool bForceSound = false);
	virtual unsigned int addStimulus(const Habit::StimulusSettings& ss, int context, bool bForceSound = false);
	virtual unsigned int addStimulus(const QString& name, const QColor& color, int context, bool bForceSound = false);
	virtual bool replaceStimulus(unsigned int key, const Habit::StimulusSettings& stimulus, bool bForceSound = false);

	unsigned int getAGKey() const { return m_agKey; };
	unsigned int getBackgroundKey() const { return m_backgroundKey; };
	unsigned int getDefaultKey() const { return m_defaultKey; };

	HStimulusWidget *getHStimulusWidget(const HPlayerPositionType& type);
	const HStimulusLayoutType& getStimulusLayoutType() const { return m_sdinfo.getStimulusLayoutType(); };
	const Habit::StimulusSettings& getStimulusSettings(unsigned int key) const;

	// Change layout type and widgets. Must supply correct # of pointers.
	void setStimulusLayoutType(const HStimulusLayoutType& layoutType, HStimulusWidget *w0, HStimulusWidget *w1);
	QList<unsigned int> getContextStimList(int context);

	QDialog *createStimulusWidget();

	// set all pipelines to paused state, then monitor and wait for all pads to be assigned.
	// emit mmReady() if all are ready; emit mmFail() if not ready before timeout ms.
	// DO NOT USE void getReady(int ms);

	// each pipeline represents a single stimulus (single or dual screen, i.e. Habit::StimulusSettings)
	const QMap<unsigned int, HPipeline *>& pipelineMap() const { return m_mapPipelines; };

	unsigned int nStimuli() const { return m_mapPipelines.size(); };

	// pipeline control
	void initialize(unsigned int id);
	void preroll(unsigned int id);
	void cleanup(unsigned int id);
	void pause(unsigned int id);
	void rewind(unsigned int id);

	// remove stimulus, delete pipeline, make lists consistent
	void remove(unsigned int id);

	// pipeline info dump
	void dump(unsigned int id);

	virtual void defaultStim() { stim(m_defaultKey); };
	virtual void background() { stim(m_backgroundKey); };
	virtual void clear();
	virtual void stop();

public Q_SLOTS:

	void stim(unsigned int);
	void ag();
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
