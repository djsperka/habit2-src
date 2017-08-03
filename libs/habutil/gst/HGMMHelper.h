/*
 * HGMMHelper.h
 *
 *  Created on: Jul 14, 2017
 *      Author: dan
 */

#ifndef APPS_HG2_HGMMHELPER_H_
#define APPS_HG2_HGMMHELPER_H_

#include <QObject>
#include <QDir>
#include <QMap>
#include <QGst/Pipeline>
#include <QGst/Element>
#include <QGst/Pad>
#include <QGst/Ui/VideoWidget>
#include "HTypes.h"
#include "stimulussettings.h"

class HGMMHelper: public QObject
{
	Q_OBJECT

	typedef void (HGMMHelper::*padAddedMember)(const QGlib::ObjectPtr & sender, const QGst::PadPtr & srcPad);

	struct HelperPath
	{
		QGst::ElementPtr sink;
		QGst::ElementPtr linkTo;
		QGst::Ui::VideoWidget *videoWidget;
		bool needPad;
		bool isLoop;
	};

	QMap<HPlayerPositionType, HelperPath> m_mapPaths;

public:
	HGMMHelper(QObject *parent=NULL);
	HGMMHelper(int id, const Habit::StimulusSettings& stimulus, const QDir& stimRoot, QGst::Ui::VideoWidget *pCenter, bool bISS=false, QObject *parent=NULL);
	HGMMHelper(int id, const Habit::StimulusSettings& stimulus, const QDir& stimRoot, QGst::Ui::VideoWidget *pLeft, QGst::Ui::VideoWidget *pRight, bool bISS=false, QObject *parent=NULL);
	HGMMHelper(const HGMMHelper& h);

	HGMMHelper& operator=(const HGMMHelper& rhs);
	bool hasPads() const;
	int id() const;
	QGst::ElementPtr& sinkLeft() { return m_sinkLeft; };
	QGst::ElementPtr& sinkRight() { return m_sinkRight; };
	QGst::ElementPtr& sinkCenter() { return m_sinkCenter; };
	QGst::PipelinePtr& pipeline() { return m_pipeline; };

	void play();

private:
	int m_id;
	const HStimulusLayoutType *m_pStimulusLayoutType;
	Habit::StimulusSettings m_stimulus;
	QDir m_root;
	bool m_bISS;
	QGst::PipelinePtr m_pipeline;
	QGst::ElementPtr m_sinkLeft, m_sinkRight, m_sinkCenter;
	bool m_bNeedPadLeft, m_bNeedPadRight,  m_bNeedPadCenter;
	QGst::ElementPtr m_linkToElementLeft, m_linkToElementRight, m_linkToElementCenter;
	QGst::Ui::VideoWidget *m_pVideoWidgetLeft, *m_pVideoWidgetRight, *m_pVideoWidgetCenter;

	// bus listener
	void onBusMessage(const QGst::MessagePtr & message);
	void handlePipelineStateChange(const QGst::StateChangedMessagePtr & scm);

	// These will create sink and connect it
	void leftPadAdded(const QGlib::ObjectPtr & sender, const QGst::PadPtr & srcPad);
	void rightPadAdded(const QGlib::ObjectPtr & sender, const QGst::PadPtr & srcPad);
	void centerPadAdded(const QGlib::ObjectPtr & sender, const QGst::PadPtr & srcPad);

	// These are for sinks that were created elsewhere and are already linked
	void setLeftSink(QGst::ElementPtr& leftSink);
	void setRightSink(QGst::ElementPtr& rightSink);
	void setCenterSink(QGst::ElementPtr& centerSink);

	// add stimulus to this helper
	void setLeftStimulus(const Habit::StimulusInfo& info, QGst::Ui::VideoWidget *pwidget);
	void setRightStimulus(const Habit::StimulusInfo& info, QGst::Ui::VideoWidget *pwidget);
	void setCenterStimulus(const Habit::StimulusInfo& info, QGst::Ui::VideoWidget *pwidget);
	void setSoundStimulus(const Habit::StimulusInfo& info, QGst::Ui::VideoWidget *pwidget);

	// use videotestsrc, static (known) sink.
	void setSolidColorStimulus(const QColor& color, QGst::ElementPtr& sink, const QString& s);

	// use decodebin and async sink pad creation
	void setUnknownStimulus(const Habit::StimulusInfo& info, QGst::ElementPtr& sink, QGst::ElementPtr& linkToElement, padAddedMember func, const QString& s);

	// jpg
	void setJPGStimulus(const Habit::StimulusInfo& info, QGst::ElementPtr& sink, const QString& s);

	// png
	void setPNGStimulus(const Habit::StimulusInfo& info, QGst::ElementPtr& sink, const QString& s);


	static GstPadProbeReturn onEvent(
			GstPad          *pad,
	        GstPadProbeInfo *info,
			gpointer         user_data);

	Q_SIGNALS:
		void nowPlaying();
};

#endif /* APPS_HG2_HGMMHELPER_H_ */
