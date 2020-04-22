/*
 * GstspDialog.cpp
 *
 *  Created on: Nov 18, 2019
 *      Author: dan
 */

#include "GstspDialog.h"
#include "HExperimentListWidget.h"
#include "HLoggerWidget.h"
#include "HDBException.h"
#include "experimentsettings.h"
#include "H2MainWindow.h"
#include "HPhaseSettings.h"
#include "HabitStimFactory.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QtDebug>
#include <QMessageBox>
#include <QList>

#include "HMM.h"
#include "GstspControlDialog.h"
#include "HWorkspaceUtil.h"
#include "HStimulusWidget.h"


GstspDialog::GstspDialog(QWidget *parent)
: QDialog(parent)
{
	qDebug() << "GstspDialog::GstspDialog()";
	components();
	qDebug() << "GstspDialog::GstspDialog()  -done";
}

GstspDialog::~GstspDialog()
{

}


void GstspDialog::components()
{
	QVBoxLayout *vbox = new QVBoxLayout;
	GUILib::HExperimentListWidget *list = new GUILib::HExperimentListWidget(this, true, true);
	vbox->addWidget(list);
	connect(list, SIGNAL(experimentActivated(const QString&)), this, SLOT(experimentActivated(const QString&)));
	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    vbox->addWidget(buttonBox);
    vbox->addWidget(new HLoggerWidget(this));

    this->setLayout(vbox);
}

void GstspDialog::experimentActivated(QString expt)
{
	qDebug() << "Got expt " << expt;

	Habit::ExperimentSettings settings;
	try
	{
		settings.loadFromDB(expt);
	}
	catch (const Habit::HDBException& e)
	{
		QMessageBox::critical(this, "Cannot load experiment", "Cannot load experiment from database!");
		qCritical() << "Cannot load experiment \"" << expt << "\" from database.";
		qCritical() << e.what();
		return;
	}

	// pre-flight check.
	QStringList sProblems;
	if (!GUILib::H2MainWindow::checkExperimentSettings(settings, sProblems))
	{
		QMessageBox msgBox;
		msgBox.setText("This experiment cannot be run.");
		msgBox.setInformativeText("Experiment settings are not complete.");
		msgBox.setDetailedText(sProblems.join("\n"));
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		msgBox.exec();
		return;
	}

	// Set up configuration for the mm port - the backend that is.
	// This is where you specify how many video outputs you will use (and you provide
	// the stimpos, a name, and the name of the gstreamer sink element).
	hmm::HMMConfiguration config;
	if (settings.getStimulusDisplayInfo().getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		config.addVideoSink(hmm::HMM::STIMPOS_CENTER);
	}
	else if (settings.getStimulusDisplayInfo().getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		config.addVideoSink(hmm::HMM::STIMPOS_LEFT);
		config.addVideoSink(hmm::HMM::STIMPOS_RIGHT);
	}
	else if (settings.getStimulusDisplayInfo().getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutTriple)
	{
		config.addVideoSink(hmm::HMM::STIMPOS_LEFT);
		config.addVideoSink(hmm::HMM::STIMPOS_RIGHT);
		config.addVideoSink(hmm::HMM::STIMPOS_CENTER);
	}

#if defined(Q_OS_MAC)
	qDebug() << "make osxaudiosink for mac";
	config.addAudioSink(hmm::HMM::STIMPOS_AUDIO,"osxaudiosink");
#elif defined(Q_OS_LINUX)
	qDebug() << "make alsasink for linux";
	config.addAudioSink(hmm::HMM::STIMPOS_AUDIO, "alsasink");
#elif defined(Q_OS_WIN)
	qDebug() << "make directsoundsink for win";
	config.addAudioSink(hmm::HMM::STIMPOS_AUDIO, "directsoundsink");
#else
	qDebug() << "Unsupported OS.";
	throw std::runtime_error("Unsupported OS, cannot create audio sink");
#endif

	//
	QDir root = habutilGetStimulusRootDir();

	// Get stim settings, add to factory, save stimid for each
	HabitStimFactory factory(settings.getStimulusDisplayInfo(), root);

	// save ag key if needed
	bool bAGUsed = (settings.getAttentionGetterSettings().isAttentionGetterUsed() || settings.getAttentionGetterSettings().isFixedISI());

	// add stimulus settings to factory...
	QListIterator<Habit::HPhaseSettings> phaseIterator = settings.phaseIterator();
	while (phaseIterator.hasNext())
	{
		const Habit::HPhaseSettings& ps = phaseIterator.next();
		const Habit::StimuliSettings& stimuliSettings = ps.stimuli();
		qDebug() << "Phase " << ps.getName();
		for (const Habit::StimulusSettings& ss: stimuliSettings.stimuli())
		{
			hmm::HMMStimID id = factory.addStimulusSettings(ss);
			m_stringlistNames.append(ss.getName());
			m_listIDs.append(id);
		}
	}

	m_pmm = new hmm::HMM(config, factory);

	// Display widget
	StimDisplayWidget *psdw = new StimDisplayWidget(settings.getStimulusDisplayInfo(), 320, 240, this);
	setWidgetPropertyOnSinks(psdw, config.video.size());
	psdw->show();
	psdw->raise();
	//psw->activateWindow();


	// create widget for control
	m_control = new GstspControlDialog(m_stringlistNames, m_listIDs, this);
	connect(m_control, &GstspControlDialog::preroll, this, &GstspDialog::preroll);
	connect(m_control, &GstspControlDialog::dump, this, &GstspDialog::dump);
	connect(m_control, &GstspControlDialog::play, this, &GstspDialog::playItem);
	m_control->exec();
	psdw->close();

	delete m_pmm;
	m_pmm = NULL;
}

void GstspDialog::setWidgetPropertyOnSinks(StimDisplayWidget *psdw, unsigned int nwidgets)
{
	if (nwidgets == 1)
	{
		GValue v = G_VALUE_INIT;
		g_value_init(&v, G_TYPE_POINTER);
		g_value_set_pointer(&v, psdw->getVideoWidget(0));
		m_pmm->port().setWidget(hmm::HMM::STIMPOS_CENTER, v);
	}
	if (nwidgets == 2)
	{
		GValue v = G_VALUE_INIT;
		g_value_init(&v, G_TYPE_POINTER);
		g_value_set_pointer(&v, psdw->getVideoWidget(0));
		m_pmm->port().setWidget(hmm::HMM::STIMPOS_LEFT, v);
		v = G_VALUE_INIT;
		g_value_init(&v, G_TYPE_POINTER);
		g_value_set_pointer(&v, psdw->getVideoWidget(1));
		m_pmm->port().setWidget(hmm::HMM::STIMPOS_RIGHT, v);
	}
	if (nwidgets == 3)
	{
		GValue v = G_VALUE_INIT;
		g_value_init(&v, G_TYPE_POINTER);
		g_value_set_pointer(&v, psdw->getVideoWidget(0));
		m_pmm->port().setWidget(hmm::HMM::STIMPOS_LEFT, v);
		v = G_VALUE_INIT;
		g_value_init(&v, G_TYPE_POINTER);
		g_value_set_pointer(&v, psdw->getVideoWidget(1));
		m_pmm->port().setWidget(hmm::HMM::STIMPOS_RIGHT, v);
		v = G_VALUE_INIT;
		g_value_init(&v, G_TYPE_POINTER);
		g_value_set_pointer(&v, psdw->getVideoWidget(2));
		m_pmm->port().setWidget(hmm::HMM::STIMPOS_CENTER, v);
	}
}

void GstspDialog::preroll(QString id)
{
	hmm::HMMInstanceID iid = m_pmm->preroll(id.toStdString());
	qDebug() << "Got instance ID " << (int)iid;
}

void GstspDialog::dump()
{
	qDebug() << "Dump dot file base name " << GST_ELEMENT_NAME(m_pmm->pipeline());
	GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(m_pmm->pipeline()), GST_DEBUG_GRAPH_SHOW_ALL, GST_ELEMENT_NAME(m_pmm->pipeline()));
}

void GstspDialog::playItem(unsigned int iid)
{
	m_pmm->play(iid);
}

void GstspDialog::stopItem()
{

}

void GstspDialog::stimStarted(int index)
{

}

void GstspDialog::agStarted(int index)
{

}

void GstspDialog::screen(int screenid, QString filename)
{

}


