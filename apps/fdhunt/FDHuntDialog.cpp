/*
 * FDHuntDialog.cpp
 *
 *  Created on: Dec 1, 2017
 *      Author: dan
 */

#include "FDHuntDialog.h"
#include "FDDialog.h"
#include <QMutexLocker>


FDHuntDialog::FDHuntDialog(PipelineData *pdata, QWidget *parent)
: QDialog(parent)
, m_pipelineData(pdata)
{
	Q_ASSERT(m_pipelineData);

	QDialogButtonBox *buttonBox;
	QVBoxLayout *vbox;
	QWidget *pWidget = new QWidget;
	pWidget->setMinimumSize(320, 240);
	buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel, Qt::Horizontal, parent);
	m_pbCleanup = new QPushButton("Cleanup");
	m_pbState = new QPushButton("State");
	m_pbPause = new QPushButton("Pause");
	m_pbRewind = new QPushButton("Rewind");
	m_pbPlay = new QPushButton("PLay");
	vbox = new QVBoxLayout;

	buttonBox->addButton(m_pbCleanup, QDialogButtonBox::ActionRole);
	buttonBox->addButton(m_pbState, QDialogButtonBox::ActionRole);
	buttonBox->addButton(m_pbPause, QDialogButtonBox::ActionRole);
	buttonBox->addButton(m_pbRewind, QDialogButtonBox::ActionRole);
	buttonBox->addButton(m_pbPlay, QDialogButtonBox::ActionRole);
	connect(m_pbState, SIGNAL(clicked()), this, SLOT(stateClicked()));
	connect(m_pbRewind, SIGNAL(clicked()), this, SLOT(rewindClicked()));
	connect(m_pbPause, SIGNAL(clicked()), this, SLOT(pauseClicked()));
	connect(m_pbPlay, SIGNAL(clicked()), this, SLOT(playClicked()));
	connect(m_pbCleanup, SIGNAL(clicked()), this, SLOT(cleanupClicked()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(accept()));
	vbox->addWidget(pWidget);
	vbox->addWidget(buttonBox);
	setLayout(vbox);

	// Connect pipeline sink to pWidget.
	// pipeline MUST have sink element named "sink"
	GstElement *sink = gst_bin_get_by_name(GST_BIN(m_pipelineData->pipeline), "sink");
	if (sink)
	{
		GValue v = G_VALUE_INIT;
		g_value_init(&v, G_TYPE_POINTER);
		g_value_set_pointer(&v, pWidget);
		g_object_set_property(G_OBJECT(sink), "widget", &v);
	}
	else
	{
		qDebug() << "No video sink - do not connect to widget.";
	}
}

FDHuntDialog::~FDHuntDialog()
{
	//delete m_paudioHGstPlayer;
	//m_pvideoHGstPlayer->stop();
	//delete m_pvideoHGstPlayer;
}

void FDHuntDialog::stateClicked()
{
	qDebug() << "Print state info...";
	if (m_pipelineData->pipeline)
		printPipelineState(m_pipelineData->pipeline);
	else
		qDebug() << "No pipeline!";
}

void FDHuntDialog::pauseClicked()
{
	qDebug() << "pause clicked";
	GstStateChangeReturn ret;
	QMutexLocker lock(&m_pipelineData->mutex);
	ret = gst_element_set_state(m_pipelineData->pipeline, GST_STATE_PAUSED);
	qDebug() << "state change returned";
	if (ret == GST_STATE_CHANGE_FAILURE)
	{
		qWarning() << "Unable to set the pipeline to the paused state";
		reject();
	}
}

void FDHuntDialog::playClicked()
{
	qDebug() << "play clicked";
	GstStateChangeReturn ret;
	ret = gst_element_set_state(m_pipelineData->pipeline, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE)
	{
		qWarning() << "Unable to set the pipeline to the playing state";
		reject();
	}
}

void FDHuntDialog::rewindClicked()
{
	qDebug() << "rewind clicked";
}

void FDHuntDialog::cleanupClicked()
{
	qDebug() << "cleanup clicked";
}


void FDHuntDialog::printPipelineState(GstElement *pipeline)
{
	GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, GST_ELEMENT_NAME(pipeline));

	auto it  = gst_bin_iterate_elements(GST_BIN(pipeline));
	GValue value = G_VALUE_INIT;
	for(GstIteratorResult r = gst_iterator_next(it, &value); r != GST_ITERATOR_DONE; r = gst_iterator_next(it, &value))
	{
		 if ( r == GST_ITERATOR_OK )
		 {
			 GstElement *e = static_cast<GstElement*>(g_value_peek_pointer(&value));
			 GstState  current, pending;
			 auto ret = gst_element_get_state(e, &current, &pending, 100000);
			 QString s("%1(%2), status = %3, pending = %4");
			 qDebug() << s.arg(QString(G_VALUE_TYPE_NAME(&value))).arg(QString(gst_element_get_name(e))).arg(QString(gst_element_state_get_name(current))).arg(QString(gst_element_state_get_name(pending)));
		 }
	}
}
