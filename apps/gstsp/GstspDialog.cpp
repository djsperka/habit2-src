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
		config.addVideoSink(hmm::HMM::STIMPOS_CENTER, "center", "qwidget5videosink");
	}
	else if (settings.getStimulusDisplayInfo().getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		config.addVideoSink(hmm::HMM::STIMPOS_LEFT, "left", "qwidget5videosink");
		config.addVideoSink(hmm::HMM::STIMPOS_RIGHT, "right", "qwidget5videosink");
	}
	else if (settings.getStimulusDisplayInfo().getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutTriple)
	{
		config.addVideoSink(hmm::HMM::STIMPOS_LEFT, "left", "qwidget5videosink");
		config.addVideoSink(hmm::HMM::STIMPOS_RIGHT, "right", "qwidget5videosink");
		config.addVideoSink(hmm::HMM::STIMPOS_CENTER, "center", "qwidget5videosink");
	}

#if defined(Q_OS_MAC)
	qDebug() << "make osxaudiosink for mac";
	config.addAudioSink(hmm::HMM::STIMPOS_AUDIO, "audio", "osxaudiosink");
#elif defined(Q_OS_LINUX)
	qDebug() << "make alsasink for linux";
	config.addAudioSink(hmm::HMM::STIMPOS_AUDIO, "audio", "alsasink");
#elif defined(Q_OS_WIN)
	qDebug() << "make directsoundsink for win";
	config.addAudioSink(hmm::HMM::STIMPOS_AUDIO, "audio", "directsoundsink");
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
			qDebug() << "Stimulus " << ss.getName();
			hmm::HMMStimID id = factory.addStimulusSettings(ss);
			m_stringlistNames.append(ss.getName());
			m_listIDs.append(id);
		}
	}

	m_pmm = new hmm::HMM(config, factory);

	// create widget for control and display
	GstspControlDialog *control = new GstspControlDialog(m_stringlistNames, m_listIDs, this);
	connect(control, &GstspControlDialog::preroll, this, &GstspDialog::preroll);
	connect(control, &GstspControlDialog::dump, this, &GstspDialog::dump);
	control->exec();

}

void GstspDialog::preroll(int id)
{
	hmm::HMMInstanceID iid = m_pmm->preroll((hmm::HMMStimID)id);
	qDebug() << "Got instance ID " << (int)iid;
}

void GstspDialog::dump()
{
	qDebug() << "Dump dot file base name " << GST_ELEMENT_NAME(m_pmm->pipeline());
	GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(m_pmm->pipeline()), GST_DEBUG_GRAPH_SHOW_ALL, GST_ELEMENT_NAME(m_pmm->pipeline()));
}

void GstspDialog::playItem(unsigned int)
{

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


