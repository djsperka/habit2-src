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

QString stateName(QGst::State s);


class HGMMHelper: public QObject
{
	Q_OBJECT

	typedef void (HGMMHelper::*padAddedMember)(const QGlib::ObjectPtr & sender, const QGst::PadPtr & srcPad);

	struct HelperPath
	{
		QGst::ElementPtr sink;
		QGst::ElementPtr linkTo;
		QGst::ElementPtr capsfilter;
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
	QGst::PipelinePtr& pipeline() { return m_pipeline; };
	void play();
	const Habit::StimulusSettings& stimulusSettings() const { return m_stimulus; };
	QGst::ElementPtr sink(const HPlayerPositionType& ppt) const;

	static QGst::ElementPtr makeElement(const char *factoryName, const HPlayerPositionType& ppt, int number);
	static QString stateName(QGst::State s);
	static const HPlayerPositionType& getPPTFromElementName(const QString& elementName);

private:
	int m_id;
	const HStimulusLayoutType *m_pStimulusLayoutType;
	Habit::StimulusSettings m_stimulus;
	QDir m_root;
	bool m_bISS;
	QGst::PipelinePtr m_pipeline;
//	QGst::ElementPtr m_sinkLeft, m_sinkRight, m_sinkCenter;
//	bool m_bNeedPadLeft, m_bNeedPadRight,  m_bNeedPadCenter;
//	QGst::ElementPtr m_linkToElementLeft, m_linkToElementRight, m_linkToElementCenter;
//	QGst::Ui::VideoWidget *m_pVideoWidgetLeft, *m_pVideoWidgetRight, *m_pVideoWidgetCenter;

	// bus listener
	void onBusMessage(const QGst::MessagePtr & message);
	void handlePipelineStateChange(const QGst::StateChangedMessagePtr & scm);

	// These will create sink and connect it
	//void leftPadAdded(const QGlib::ObjectPtr & sender, const QGst::PadPtr & srcPad);
	//void rightPadAdded(const QGlib::ObjectPtr & sender, const QGst::PadPtr & srcPad);
	//void centerPadAdded(const QGlib::ObjectPtr & sender, const QGst::PadPtr & srcPad);
	void padAdded(const QGlib::ObjectPtr & sender, const QGst::PadPtr & srcPad);

	// These are for sinks that were created elsewhere and are already linked
//	void setLeftSink(QGst::ElementPtr& leftSink);
//	void setRightSink(QGst::ElementPtr& rightSink);
//	void setCenterSink(QGst::ElementPtr& centerSink);

	// add stimulus to this helper
	void setStimulus(const Habit::StimulusInfo& info, QGst::Ui::VideoWidget *pwidget, const HPlayerPositionType& ppt);

	// use videotestsrc, static (known) sink.
	void setSolidColorStimulus(const QColor& color, const HPlayerPositionType& ppt);

	// use decodebin and async sink pad creation
	void setUnknownStimulus(const Habit::StimulusInfo& info, const HPlayerPositionType& ppt);

	// jpg
	void setJPGStimulus(const Habit::StimulusInfo& info, const HPlayerPositionType& ppt);

	// png
	void setPNGStimulus(const Habit::StimulusInfo& info, const HPlayerPositionType& ppt);


	static GstPadProbeReturn onEvent(
			GstPad          *pad,
	        GstPadProbeInfo *info,
			gpointer         user_data);

	Q_SIGNALS:
		void nowPlaying();
};

#endif /* APPS_HG2_HGMMHELPER_H_ */
