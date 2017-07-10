/*
 * HGstMediaManager.h
 *
 *  Created on: Jun 14, 2017
 *      Author: dan
 */

#ifndef LIBS_HABUTIL_HGSTMEDIAMANAGER_H_
#define LIBS_HABUTIL_HGSTMEDIAMANAGER_H_

#include "HMediaManager.h"
#include "HTypes.h"
#include <QGst/Pipeline>
#include <QGst/Ui/VideoWidget>
#include <QGst/Pad>
#include <QGst/Bin>
#include <QGst/Element>


class HGstMediaManager: public HMediaManager
{
	Q_OBJECT

private:

	const HStimulusLayoutType& m_stimulusLayoutType;
	QGst::Ui::VideoWidget *m_pVideoWidgetCenter;
	QGst::Ui::VideoWidget *m_pVideoWidgetLeft;
	QGst::Ui::VideoWidget *m_pVideoWidgetRight;

	struct HGstMMData
	{
		QGst::PipelinePtr m_pipeline;
		QGst::ElementPtr m_sink;
	};

	typedef QMap< unsigned int, HGstMMData > HGstMMDataMap;

	HGstMMDataMap m_hgstDataMap;

	virtual unsigned int addStimulusPrivate(unsigned int key, const Habit::StimulusSettings& ss);

public:
	//HGstMediaManager(const HStimulusLayoutType& slType, bool bISS=false);
	HGstMediaManager(const HStimulusLayoutType& slType, QGst::Ui::VideoWidget *v, bool bISS=false);
	//HGstMediaManager(const HStimulusLayoutType& slType, QGst::Ui::VideoWidgetPtr vleft, QGst::Ui::VideoWidgetPtr vRight, bool bISS=false);
	virtual ~HGstMediaManager();
	virtual void clear();
	virtual unsigned int addAG(const Habit::StimulusSettings& ags);

public Q_SLOTS:

	virtual void stim(int);

};

#endif /* LIBS_HABUTIL_HGSTMEDIAMANAGER_H_ */
