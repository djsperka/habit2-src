/*
 * HRunSettingsDialog.cpp
 *
 *  Created on: Dec 1, 2014
 *      Author: Oakeslab
 */

#include "HRunSettingsDialog.h"
#include "HStimulusOrderSelectionWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

GUILib::HRunSettingsDialog::HRunSettingsDialog(const Habit::ExperimentSettings& s, bool bTestRun, QWidget* parent)
: QDialog(parent)
, m_exptSettings(s)
, m_bPreTestOrderChosen(false)
, m_bHabituationOrderChosen(false)
, m_bTestOrderChosen(false)
{
	components(bTestRun);
	connections();
}


QString GUILib::HRunSettingsDialog::getRunLabel() const
{
	QString subjectName("TEST");
	if (!m_pSubjectSettingsWidget->isTestRun())
		subjectName = m_pSubjectSettingsWidget->getSubjectID();
	return QString("%1_%2_%3").arg(m_exptSettings.getName()).arg(subjectName).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd-hhmm"));
}


Habit::SubjectSettings GUILib::HRunSettingsDialog::getSubjectSettings() const
{
	Habit::SubjectSettings settings;
	settings.setComments(m_pSubjectSettingsWidget->getComments());
	settings.setSubjectName(m_pSubjectSettingsWidget->getSubjectID());
	settings.setObserver(m_pSubjectSettingsWidget->getObserverID());
	return settings;
}


Habit::RunSettings GUILib::HRunSettingsDialog::getRunSettings() const
{
	Habit::RunSettings settings;
	settings.setExperimentId(m_exptSettings.getId());
	settings.setSubjectId(-1);	// unused

	if (m_map.contains(HStimContext::PreTestPhase.number()))
	{
		settings.setPretestRandomized(m_map[HStimContext::PreTestPhase.number()]->isRandomized());
		settings.setPretestRandomizeMethod(m_map[HStimContext::PreTestPhase.number()]->getRandomizationType().number());

		// determine the order to use and generate a QList<int>
		QList<QPair<int, QString> > list;
		if (m_map[HStimContext::PreTestPhase.number()]->isDefaultOrder())
		{
			int nstim = m_exptSettings.getPreTestStimuliSettings().stimuli().size();
			for (int i=0; i<nstim; i++)
				list.append(QPair<int, QString>(i, QString()));
		}
		else if (m_map[HStimContext::PreTestPhase.number()]->isDefinedOrder())
		{
			settings.setPretestOrderName(m_map[HStimContext::PreTestPhase.number()]->getDefinedOrderName());
			if (!m_exptSettings.getPreTestStimuliSettings().getIndexedOrderList(m_map[HStimContext::PreTestPhase.number()]->getDefinedOrderName(), list))
				qCritical() << 	"HRunSettingsDialog::getRunSettings(): Cannot find defined order \"" <<
								m_map[HStimContext::PreTestPhase.number()]->getDefinedOrderName() <<
								"\" in pretest stimuli settings!";
		}
		settings.setPretestOrderList(list);
	}

	if (m_map.contains(HStimContext::HabituationPhase.number()))
	{
		settings.setHabituationRandomized(m_map[HStimContext::HabituationPhase.number()]->isRandomized());
		settings.setHabituationRandomizeMethod(m_map[HStimContext::HabituationPhase.number()]->getRandomizationType().number());

		// determine the order to use and generate a QList<int>
		QList<QPair<int, QString> > list;
		if (m_map[HStimContext::HabituationPhase.number()]->isDefaultOrder())
		{
			int nstim = m_exptSettings.getHabituationStimuliSettings().stimuli().size();
			for (int i=0; i<nstim; i++)
				list.append(QPair<int, QString>(i, QString()));
		}
		else if (m_map[HStimContext::HabituationPhase.number()]->isDefinedOrder())
		{
			settings.setHabituationOrderName(m_map[HStimContext::HabituationPhase.number()]->getDefinedOrderName());
			if (!m_exptSettings.getHabituationStimuliSettings().getIndexedOrderList(m_map[HStimContext::HabituationPhase.number()]->getDefinedOrderName(), list))
				qCritical() << 	"HRunSettingsDialog::getRunSettings(): Cannot find defined order \"" <<
								m_map[HStimContext::HabituationPhase.number()]->getDefinedOrderName() <<
								"\" in habituation stimuli settings!";
		}
		settings.setHabituationOrderList(list);
	}

	if (m_map.contains(HStimContext::TestPhase.number()))
	{
		settings.setTestRandomized(m_map[HStimContext::TestPhase.number()]->isRandomized());
		settings.setTestRandomizeMethod(m_map[HStimContext::TestPhase.number()]->getRandomizationType().number());

		// determine the order to use and generate a QList<int>
		// Habit uses 1-based indexes!
		QList<QPair<int, QString> > list;
		if (m_map[HStimContext::TestPhase.number()]->isDefaultOrder())
		{
			int nstim = m_exptSettings.getTestStimuliSettings().stimuli().size();
			for (int i=0; i<nstim; i++)
				list.append(QPair<int, QString>(i, QString()));
		}
		else if (m_map[HStimContext::TestPhase.number()]->isDefinedOrder())
		{
			settings.setTestOrderName(m_map[HStimContext::TestPhase.number()]->getDefinedOrderName());
			if (!m_exptSettings.getTestStimuliSettings().getIndexedOrderList(m_map[HStimContext::TestPhase.number()]->getDefinedOrderName(), list))
				qCritical() << 	"HRunSettingsDialog::getRunSettings(): Cannot find defined order \"" <<
								m_map[HStimContext::TestPhase.number()]->getDefinedOrderName() <<
								"\" in test stimuli settings!";
		}
		settings.setTestOrderList(list);
	}

	return settings;
}


void GUILib::HRunSettingsDialog::components(bool bTestRun)
{
	QVBoxLayout *v = new QVBoxLayout;

	// subject setting widget
	m_pSubjectSettingsWidget = new HSubjectSettingsWidget(bTestRun, this);
	v->addWidget(m_pSubjectSettingsWidget);

	// pretest?
	if (m_exptSettings.getPreTestPhaseSettings().getIsEnabled())
	{
		GUILib::HStimulusOrderSelectionWidget* w = new HStimulusOrderSelectionWidget(m_exptSettings.getPreTestStimuliSettings(), this);
		v->addWidget(w);
		m_map.insert(HStimContext::PreTestPhase.number(), w);
	}

	// habituation?
	if (m_exptSettings.getHabituationPhaseSettings().getIsEnabled())
	{
		GUILib::HStimulusOrderSelectionWidget* w = new HStimulusOrderSelectionWidget(m_exptSettings.getHabituationStimuliSettings(), this);
		v->addWidget(w);
		m_map.insert(HStimContext::HabituationPhase.number(), w);
	}

	// test?
	if (m_exptSettings.getTestPhaseSettings().getIsEnabled())
	{
		GUILib::HStimulusOrderSelectionWidget* w = new HStimulusOrderSelectionWidget(m_exptSettings.getTestStimuliSettings(), this);
		v->addWidget(w);
		m_map.insert(HStimContext::TestPhase.number(), w);
	}


	// buttons
	QHBoxLayout *h = new QHBoxLayout;
	h->addStretch(1);
	m_pbCancel = new QPushButton("Cancel");
	h->addWidget(m_pbCancel);
	m_pbRun = new QPushButton("Run");
	h->addWidget(m_pbRun);
	v->addLayout(h);

	// set layout for the dialog
	setLayout(v);

	// Initial state of run button has to be disabled
	m_pbRun->setEnabled(false);

}

void GUILib::HRunSettingsDialog::connections()
{
	connect(m_pbCancel, SIGNAL(clicked()), this, SLOT(reject()));
	connect(m_pbRun, SIGNAL(clicked()), this, SLOT(accept()));
	QMapIterator<int, GUILib::HStimulusOrderSelectionWidget*> it(m_map);
	while (it.hasNext())
	{
		it.next();
		if (it.key() == HStimContext::PreTestPhase.number())
		{
			connect(it.value(), SIGNAL(orderChosen()), this, SLOT(preTestOrderChosen()));
		}
		if (it.key() == HStimContext::HabituationPhase.number())
		{
			connect(it.value(), SIGNAL(orderChosen()), this, SLOT(habituationOrderChosen()));
		}
		if (it.key() == HStimContext::TestPhase.number())
		{
			connect(it.value(), SIGNAL(orderChosen()), this, SLOT(testOrderChosen()));
		}
	}
}

void GUILib::HRunSettingsDialog::preTestOrderChosen()
{
	m_bPreTestOrderChosen = true;
	updateRunButton();
}

void GUILib::HRunSettingsDialog::habituationOrderChosen()
{
	m_bHabituationOrderChosen = true;
	updateRunButton();
}

void GUILib::HRunSettingsDialog::testOrderChosen()
{
	m_bTestOrderChosen = true;
	updateRunButton();
}

void GUILib::HRunSettingsDialog::updateRunButton()
{
	bool bEnable = true;	// enable unless we find a selection widget without an order chosen
	QMapIterator<int, GUILib::HStimulusOrderSelectionWidget*> it(m_map);
	while (it.hasNext())
	{
		it.next();
		if (it.key() == HStimContext::PreTestPhase.number() && !m_bPreTestOrderChosen)
		{
			bEnable = false;
		}
		else if (it.key() == HStimContext::HabituationPhase.number() && !m_bHabituationOrderChosen)
		{
			bEnable = false;
		}
		else if (it.key() == HStimContext::TestPhase.number() && !m_bTestOrderChosen)
		{
			bEnable = false;
		}
	}
	m_pbRun->setEnabled(bEnable);
}
