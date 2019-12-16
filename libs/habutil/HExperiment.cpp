/*
 * HExperiment.cpp
 *
 *  Created on: Oct 6, 2015
 *      Author: Oakeslab
 */

#include "HExperiment.h"
#include "HElapsedTimer.h"
#include "HLookDetector.h"
#include "HGMM.h"
#include <QObject>
#include <algorithm>


HExperiment::HExperiment(HEventLog& log, HLookDetector& ld, QState* parent)
: HLogState(log, "HExperiment", parent)
, m_ld(ld)
{
	connect(&m_ld, SIGNAL(attention()), this, SLOT(onAttention()));
	connect(&m_ld, SIGNAL(look(HLook)), this, SLOT(onLook(HLook)));
	connect(&m_ld, SIGNAL(lookAborted(HLook)), this, SLOT(onLookAborted(HLook)));
};

void HExperiment::onAttention()
{
	eventLog().append(new HAttentionEvent(HElapsedTimer::elapsed()));
}


void HExperiment::onLook(HLook l)
{
	eventLog().append(new HLookEvent(l, HElapsedTimer::elapsed()));
}

void HExperiment::onLookAborted(HLook l)
{
	eventLog().append(new HIncompleteLookEvent(l, HElapsedTimer::elapsed()));
}

void HExperiment::onPhaseStarted(QString phase)
{
	qDebug() << "HExperiment::phaseStarted( " << phase << ")";
}

void HExperiment::onPhaseEnded(QString phase)
{
	qDebug() << "HExperiment::phaseEnded( " << phase << ")";
}

QDebug operator<<(QDebug dbg, const PhaseStimStuff& pss)
{
	dbg.nospace() << "Context " << pss.context << " hab? " << pss.isHabituation << endl;
	dbg.nospace() << pss.sslist;
	return dbg.space();
}


void HExperiment::onExit(QEvent* e)
{
	Q_UNUSED(e);
	// on exiting experiment, tell Media Manager to put up blank screen.
	HGMM::instance().defaultStim();
};

void HExperiment::onEntry(QEvent *)
{
//		qDebug() << "HExperiment - phase stimuli lists";
//		qDebug() << m_phaseStimulusLists;
}


QDebug operator<<(QDebug dbg, const PhT& pht)
{
	dbg.nospace() << "PhT: (" << pht.context << "," << pht.trial << ")";
	return dbg.nospace();
}


void HExperiment::requestStim(int context, unsigned int trial)
{
	PhT key(context, trial);
	PhTStimidMap::iterator it = m_prerolled.find(key);
	if (it != m_prerolled.end())
	{
		eventLog().append(new HStimulusSettingsEvent(getStimulusSettings(key), it.value(), HElapsedTimer::elapsed()));
		eventLog().append(new HStimRequestEvent(it.value(), HElapsedTimer::elapsed()));
		HGMM::instance().stim(it.value());
	}
	else
		qCritical() << "requestStim(" << context << ", " << trial << ") This stim is not prerolled!";
}

void HExperiment::requestCleanup(int context, unsigned int trial)
{
	// find in preroll list
	PhT key(context, trial);
	PhTStimidMap::iterator it = m_prerolled.find(key);
	if (it != m_prerolled.end())
	{
		m_cleanup.append(qMakePair(key, it.value()));
		m_prerolled.remove(key);
	}
}

void HExperiment::requestCleanup(int context)
{
	// must iterate through all and find those in this context.

	for(PhTStimidMap::iterator it = m_prerolled.begin(); it != m_prerolled.end(); )
	{
		if (it.key().context == context)
		{
			m_cleanup.append(qMakePair(it.key(), it.value()));
			it = m_prerolled.erase(it);
		}
		else
		{
			++it;
	    }
	}
}

void HExperiment::prerollAsNeeded(int context, unsigned int trial)
{
	prerollList(nextTrials(context, trial));
}

void HExperiment::prerollAsNeeded(int context)
{
	prerollList(nextTrials(context, 0));
}

void HExperiment::prerollNextPhase(int context_current)
{
	// get context of next phase, if any
	PhaseStimulusLists::iterator it;
	it = std::find_if(m_phaseStimulusLists.begin(), m_phaseStimulusLists.end(), [context_current](const PhaseStimStuff& psstuff) { return psstuff.context == context_current; });
	if (it != m_phaseStimulusLists.end())
	{
		it++;
		if (it != m_phaseStimulusLists.end())
		{
			qDebug() << "HExperiment::prerollNextPhase(current=" << context_current << "): found next context " << it->context;
			prerollAsNeeded(it->context);
		}
		else
		{
			qDebug() << "HExperiment::prerollNextPhase(current=" << context_current << "): this is the last phase";
		}
	}
	else
	{
		qWarning() << "HExperiment::prerollNextPhase(current=" << context_current << "): this phase not found.";
	}
}

void HExperiment::addPhaseStimulusList(int context, const Habit::HStimulusSettingsList& sslist, bool isHab)
{
	m_phaseStimulusLists.append(PhaseStimStuff(sslist, context, isHab));
}

void HExperiment::printLists()
{
	foreach (auto stuff, m_phaseStimulusLists)
	{
		qDebug() << "Context " << stuff.context;
		qDebug() << "Stimuli in order";
		qDebug() << stuff;
		qDebug() << "=====";
	}
}

void HExperiment::prerollList(const PhTList& l)
{
	foreach(PhT pht, l)
	{
		// check if its already prerolled
		if (!m_prerolled.contains(pht))
		{
			unsigned int i = HGMM::instance().addStimulus(getStimulusSettings(pht), pht.context);
			HGMM::instance().preroll(i);
			m_prerolled.insert(pht, i);
			//qDebug() << "HExperiment::prerollList: prerolling " << pht << " stimid " << i;
		}
		else
		{
			//qDebug() << "HExperiment::prerollList: " << pht << " already prerolled";
		}
	}
}

Habit::StimulusSettings HExperiment::getStimulusSettings(const PhT& pht)
{
	//verify that we can find the context and trial in m_phaseStimulusLists
	PhaseStimulusLists::iterator it;
	it = std::find_if(m_phaseStimulusLists.begin(), m_phaseStimulusLists.end(), [pht](const PhaseStimStuff& psstuff) { return psstuff.context == pht.context; });
	if (it == m_phaseStimulusLists.end())
	{
		qCritical() << "HExperiment::getStimulusSettings: Context/trial " << pht << " not found.";
		Q_ASSERT(false);
	}
	return it->sslist.at(pht.trial);

}

PhTList HExperiment::nextTrials(int context, int trial, int n)
{
	PhTList l;
	PhaseStimulusLists::iterator it;
	it = std::find_if(m_phaseStimulusLists.begin(), m_phaseStimulusLists.end(), [context](const PhaseStimStuff& psstuff) { return psstuff.context == context; });
	if (it != m_phaseStimulusLists.end())
	{
		int istart = trial;
		bool bDone = false;	// set to true when we want to get something from
							// the next phase but there are no  more phases
		while (l.size()<n && !bDone)
		{
			for (int i=istart; i < it->sslist.size() && l.size() < n; i++)
				l.append(PhT(it->context, i));
			if (l.size() < n)
			{
				if (++it != m_phaseStimulusLists.end())
					istart = 0;
				else
					bDone = true;
			}
		}

		if (!bDone && it->isHabituation)
		{
			if (++it != m_phaseStimulusLists.end())
			{
				PhTList lhab;
				lhab = nextTrials(it->context);
				l.append(lhab);
			}
		}
	}

//	qDebug() << "nextTrials(" << context << ", " << trial << ", " << n;
//	qDebug() << l;
	return l;
}


