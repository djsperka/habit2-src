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
#include <QGst/Ui/VideoWidget>
#include <QGst/Element>
#include <QGst/Pipeline>
#include <QGst/Pad>
#include "HGMMHelper.h"
#include "HTypes.h"
#include "stimulussettings.h"
#include "stimulisettings.h"

class HGMM: public QObject
{
	Q_OBJECT

	const HStimulusLayoutType& m_stimulusLayoutType;
	QDir m_root;
	bool m_bUseISS;
	QGst::Ui::VideoWidget *m_pCenter, *m_pLeft, *m_pRight;
	QTimer *m_readyTimeout, *m_readyCheck;
	HGMMHelper *m_pHelperCurrent;

	// A map containing lists of stimulus keys (keys into m_mapStim). Here the key
	// is HStimContext.
	// getContextStimList fetches a list of keys for the stimuli added for the Test phase.
	//QMap<HStimContext, QList<unsigned int> > m_mapContext;
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
	HGMM(QGst::Ui::VideoWidget *center, const QDir& dir = QDir("/"), bool useISS = true);
	HGMM(QGst::Ui::VideoWidget *left, QGst::Ui::VideoWidget *right, const QDir& dir = QDir("/"), bool useISS = true);
	virtual ~HGMM();


	virtual void clear();
	virtual unsigned int addAG(const Habit::StimulusSettings& ags);
	virtual unsigned int addBackground(const QColor& color);
	virtual void addStimuli(const Habit::StimuliSettings& ss, int context);
	virtual unsigned int addStimulus(const Habit::StimulusSettings& ss, int context);
	virtual unsigned int addStimulus(const QString& name, const QColor& color, int context);


	const Habit::StimulusSettings& getStimulusSettings(int key) const;
	QList<unsigned int> getContextStimList(int context);

	// set all pipelines to paused state, then monior and wait for all pads to be assigned.
	// emit mmReady() if all are ready; emit mmFail() if not ready before timeout ms.
	void getReady(int ms);

	//const QMap<unsigned int, const Habit::StimulusSettings *>& pmap() { return m_mapPStimulusSettings; };

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

};
#endif /* APPS_HG2_HGMM_H_ */
