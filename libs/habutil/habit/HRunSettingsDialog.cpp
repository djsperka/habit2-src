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
#include <QString>

GUILib::HRunSettingsDialog::HRunSettingsDialog(const Habit::ExperimentSettings& s, bool bTestRun, QWidget* parent)
: QDialog(parent)
, m_exptSettings(s)
{
	setWindowTitle(QString("Run Settings for Experiment: %1").arg(s.getName()));
	components(bTestRun);
	connections();
}

GUILib::HRunSettingsDialog::~HRunSettingsDialog()
{
	qDebug() << "GUILib::HRunSettingsDialog::~HRunSettingsDialog()";
}

QString GUILib::HRunSettingsDialog::getRunLabel() const
{
	QString subjectName(m_pSubjectSettingsWidget->getSubjectID());
	if (subjectName.isEmpty()) subjectName = "TEST";
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

	QMapIterator<int, QPair<GUILib::HStimulusOrderSelectionWidget*, bool> > iterator(m_map);
	Habit::PhaseRunSettings prs;

	while (iterator.hasNext())
	{
		Habit::StimLabelList list;
		int seqno;
		GUILib::HStimulusOrderSelectionWidget* w;

		iterator.next();
		seqno = iterator.key();
		w = iterator.value().first;
		prs.setOrderRandomized(w->isRandomized());
		prs.setRandomizeMethod(w->getRandomizationType().number());
		list.clear();
		if (w->isDefaultOrder())
		{
			Q_ASSERT(m_exptSettings.phaseExists(seqno) >= 0);
			int nstim = m_exptSettings.phaseAt(m_exptSettings.phaseExists(seqno)).stimuli().stimuli().size();
			for (int i=0; i<nstim; i++)
				list.append(QPair<int, QString>(i, QString()));
		}
		else if (w->isDefinedOrder())
		{
			Q_ASSERT(m_exptSettings.phaseExists(seqno) >= 0);
			prs.setOrderName(w->getDefinedOrderName());
			if (!m_exptSettings.phaseAt(m_exptSettings.phaseExists(seqno)).stimuli().getIndexedOrderList(w->getDefinedOrderName(), list))
				qCritical() << 	"HRunSettingsDialog::getRunSettings(): Cannot find defined order \"" <<
								w->getDefinedOrderName() <<
								"\" in stimuli settings for phase " << m_exptSettings.phaseAt(m_exptSettings.phaseExists(seqno)).getName() << "!";
		}
		prs.setOrderList(list);
		settings.insert(seqno, prs);
	}

	return settings;
}

bool GUILib::HRunSettingsDialog::isDisplayStimInWindow() const
{
	return m_pRunSettingsTestingWidget->isDisplayStimInWindow();
}

void GUILib::HRunSettingsDialog::components(bool bTestRun)
{
	QVBoxLayout *v = new QVBoxLayout;

	// testing options
	m_pRunSettingsTestingWidget = new GUILib::HRunSettingsTestingWidget(this);
	v->addWidget(m_pRunSettingsTestingWidget);

	// subject setting widget
	m_pSubjectSettingsWidget = new HSubjectSettingsWidget(bTestRun, this);
	v->addWidget(m_pSubjectSettingsWidget);

	// iterate over phases that are enabled
	QListIterator<Habit::HPhaseSettings> iterator = m_exptSettings.phaseIterator();
	while (iterator.hasNext())
	{
		const Habit::HPhaseSettings& ps = iterator.next();
		if (ps.getIsEnabled())
		{
			GUILib::HStimulusOrderSelectionWidget* w = new HStimulusOrderSelectionWidget(ps.stimuli(), ps.getName(), ps.getSeqno(), this);
			v->addWidget(w);
			m_map.insert(ps.getSeqno(), QPair<GUILib::HStimulusOrderSelectionWidget*, bool>(w, false));
		}
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
	QMapIterator<int, QPair<GUILib::HStimulusOrderSelectionWidget*, bool> > it(m_map);
	while (it.hasNext())
	{
		it.next();
		connect(it.value().first, SIGNAL(orderChosen(int)), this, SLOT(orderChosen(int)));
	}
}

void GUILib::HRunSettingsDialog::orderChosen(int seqno)
{
	m_map.insert(seqno, qMakePair(m_map.value(seqno).first, true));
	updateRunButton();
}


void GUILib::HRunSettingsDialog::updateRunButton()
{
	bool bEnable = true;	// enable unless we find a selection widget without an order chosen
	QMapIterator<int, QPair<GUILib::HStimulusOrderSelectionWidget*, bool> > it(m_map);
	while (it.hasNext())
	{
		it.next();
		if (!it.value().second) bEnable = false;
	}
	m_pbRun->setEnabled(bEnable);
}
