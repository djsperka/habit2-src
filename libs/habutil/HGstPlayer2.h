/*
 * HGstPlayer2.h
 *
 *  Created on: Apr 3, 2017
 *      Author: dan
 */

#ifndef LIBS_HABUTIL_HGSTPLAYER_H_
#define LIBS_HABUTIL_HGSTPLAYER_H_

#include "HPlayer.h"
#include "HTypes.h"
#include <QGst/Clock>
#include <QGst/Pipeline>
#include <QGst/Ui/VideoWidget>
#include <QGst/Pad>
#include <QGst/Bin>
#include <QGst/Element>
#include <QGst/Discoverer>
#include <QMap>

class HGstPlayer2: public HPlayer
{
	Q_OBJECT

public:
	struct QGstPtrs
	{
		QGst::PadPtr videoSrcPadPtr;		// this is the source pad that we block to stop the flow when not connected
		QGst::PadPtr inputSelectorPadPtr;	// this is the pad on the input selector that this source connects to
		QGst::PadPtr vSrcPadPtr;			// the src pad for the entire bin that provides the video. This connects to sink, or is blocked.
		unsigned int uiVideoPadProbeID;		// when videoSrcPadPtr is
		//bool bVideo
		QGst::PadPtr audioPadPtr;
		unsigned int uiAudioPadProbeID;
	};

private:
	typedef QMap< unsigned int, QGstPtrs > QGstPtrsMap;

	QGst::PipelinePtr m_pipeline;
	QGst::ElementPtr m_sink;
	QGst::ElementPtr m_input_selector;
	QGstPtrsMap m_pads;
	unsigned int m_iCurrentStimulusId;
	bool m_hasCurrent;	// set to true after first stim started
	unsigned int m_iPendingStimulusId;

	unsigned int addStimulusPrivate(unsigned int id);
	//unsigned int addBinFromDescription(unsigned int id, const QString& description);
	unsigned int addBackground(unsigned int id, const QColor& bkgdColor);
	void verifyStreamInfo(QGst::DiscovererInfoPtr info);
    void onBusMessage(const QGst::MessagePtr & message);
    void decodebinPadAdded(const QGlib::ObjectPtr & sender, const QGst::PadPtr & pad);
    const HMediaContainerType& getMediaContainerType(const QString& filename);
	void setActivePad(unsigned int num);
	void setActivePad(void);
	static GstPadProbeReturn cb_blocked_active(
			GstPad          *pad,
	        GstPadProbeInfo *info,
			gpointer         user_data);

	static GstPadProbeReturn cb_blocked(
			GstPad          *pad,
	        GstPadProbeInfo *info,
			gpointer         user_data);


public:
	HGstPlayer2(int ID = 0, QWidget* w = 0, const QDir& dir = QDir("/"));
	virtual ~HGstPlayer2();

	/// Play the stim at index 'number'. Out of range index defaults to background.
	virtual void play(unsigned int number);

	// set pipeline to PAUSED state
	void pause();

	/// Stop playing the current stim. Has no effect on a video that has stopped, or
	/// on an image or background.
	virtual void stop();

	/// Clear screen or equivalent. Emit cleared() when operation complete.
	virtual void clear();

	/// Get pipeline pointer
	QGst::PipelinePtr& pipeline() { return m_pipeline; }

	/// tell player to work with this widget
	void setVideoWidget(QGst::Ui::VideoWidget *p) { p->setVideoSink(m_sink); };

};

#endif /* LIBS_HABUTIL_HGSTPLAYER_H_ */
