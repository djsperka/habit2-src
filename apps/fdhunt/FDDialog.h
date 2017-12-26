/*
 * FDDialog.h
 *
 *  Created on: Dec 1, 2017
 *      Author: dan
 */

#ifndef APPS_FDHUNT_FDDIALOG_H_
#define APPS_FDHUNT_FDDIALOG_H_


#include <QtWidgets>
#include <gst/gst.h>
#include <glib.h>
#include <QMutex>

struct PipelineData
{
	GstElement *pipeline;
	bool bLoop;
	int probeID;
	bool bWaitingForFlushingSeekCompletion;
	bool bPrerolled;
	bool bAudio;			// audio associated with container source file, not independent audio file
	bool bVideo;			// if false, ignore video (and delete the video sink). Otherwise, link video
	QMutex mutex;		// must get this when seeking or changing state of pipeline to prevent deadlock

	// constructor
	PipelineData();
	~PipelineData();
};

class FDDialog: public QDialog
{
	Q_OBJECT

public:
	FDDialog(QWidget *parent = NULL);
	virtual ~FDDialog();

protected Q_SLOTS:
	void goClicked();

private:
	GThread *m_gthread;
	GMainLoop *m_pgml;

	QPushButton *m_pbGo;
	QListWidget *m_pListWidget;

	PipelineData *createPipeline(int);
	PipelineData *createTestSrcPipeline();
	PipelineData *createTestSrcSolidColorPipeline();
	PipelineData *createDecodebinImagePipeline();
	PipelineData *createDecodebinVideoPipeline(const char *movieFileName, bool bLoop, bool bAudio, bool bVideo=true);

	static void imagePadAdded(GstElement *src, GstPad *new_pad, PipelineData *pdata);
	static void videoPadAdded(GstElement *src, GstPad *new_pad, PipelineData *pdata);
	static void stimPadAdded(GstElement *src, GstPad *new_pad, PipelineData *pdata);
	static void parseCaps(GstCaps* caps, bool& isVideo, bool& isImage, int& width, int& height, bool& isAudio);
	static gpointer threadFunc(gpointer user_data);
	static gboolean busCallback(GstBus *, GstMessage *msg, gpointer p);
	static GstPadProbeReturn eventProbeCB(GstPad * pad, GstPadProbeInfo * info, gpointer p);
	static GstPadProbeReturn audioSinkEventProbeCB(GstPad * pad, GstPadProbeInfo * info, gpointer p);
	static GstPadProbeReturn eventProbeDoNothingCB(GstPad * pad, GstPadProbeInfo * info, gpointer p);
	static GstPadProbeReturn eventProbeAudioDoNothingCB(GstPad * pad, GstPadProbeInfo * info, gpointer p);
	//static GstPadProbeReturn eventProbeDoNothingCBitest(GstPad * pad, GstPadProbeInfo * info, gpointer p, int itest);

};




#endif /* APPS_FDHUNT_FDDIALOG_H_ */
