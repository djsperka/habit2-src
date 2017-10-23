/*
 * HVideoWidget.cpp
 *
 *  Created on: Aug 23, 2017
 *      Author: dan
 */

#include <hgst/HVideoWidget.h>
#include <QGst/Element>
#include <QGst/Pad>
#include <QGst/Caps>
#include <QGst/Structure>
#include <QSizePolicy>


HVideoWidget::HVideoWidget(const HDisplayType& displayType, bool bMaintain, QWidget *parent)
: QGst::Ui::VideoWidget(parent)
, m_sizeStimulus(320, 240)
, m_displayType(displayType)
, m_bMaintainAspectRatio(bMaintain)
{
	//setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	//setMinimumSize(QSize(320,240));
}

HVideoWidget::~HVideoWidget()
{
}

void HVideoWidget::stimulusChanged()
{
	qDebug() << "stimulus changed";
#if 0
	QGst::ElementPtr sink = videoSink();
    if (sink)
    {
    		QGst::CapsPtr caps = sink->getStaticPad("sink")->currentCaps();
    		QGst::StructurePtr structure = caps->internalStructure(0);
    		m_sizeStimulus.setWidth(structure->value("width").toInt());
    		m_sizeStimulus.setHeight(structure->value("height").toInt());
    		qDebug() << "stimulusChanged: new size " << m_sizeStimulus <<  " caps " << caps->toString();
    		qDebug() << "parent size "  << parentWidget()->size() << " my size " << size();
    		setFixedSize(getSize());
    }
    updateGeometry();
	qDebug() << "update size done - parent size "  << parentWidget()->size() << " my size " << size();
#endif
	return;
}

//QSize HVideoWidget::sizeHint() const
//QSize HVideoWidget::getSize() const
//{
//	if (m_displayType == HDisplayType::HDisplayTypeFullScreen && m_bMaintainAspectRatio)
//	{
//		qDebug() << "sizeHint - full/maintain " << m_sizeStimulus.scaled(parentWidget()->size(), Qt::KeepAspectRatio);
//		return m_sizeStimulus.scaled(parentWidget()->size(), Qt::KeepAspectRatio);
//	}
//	else if (m_displayType == HDisplayType::HDisplayTypeFullScreen && !m_bMaintainAspectRatio)
//	{
//		qDebug() << "sizeHint - full/!maintain " << parentWidget()->size();
//		return parentWidget()->size();
//	}
//	else if (m_displayType == HDisplayType::HDisplayTypeOriginalSize)
//	{
//		qDebug() << "sizeHint - original " << m_sizeStimulus;
//		return m_sizeStimulus;
//	}
//	else
//	{
//		qDebug() << "sizeHint - ??? " << m_sizeStimulus;
//		return m_sizeStimulus;
//	}
//}
