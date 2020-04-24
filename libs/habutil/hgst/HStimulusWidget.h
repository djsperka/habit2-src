/*
 * HVideoWidget.h
 *
 *  Created on: Aug 17, 2017
 *      Author: dan
 */

#ifndef LIBS_HABUTIL_HGST_HSTIMULUSWIDGET_H_
#define LIBS_HABUTIL_HGST_HSTIMULUSWIDGET_H_

#include <QWidget>
#include <QFrame>
#include <QHBoxLayout>
#include "HVideoWidget.h"
#include "HTypes.h"
#include "stimulusdisplayinfo.h"


// HVideoWidget is the widget used in Habit to display a single visual stimulus.
// It consists of a widget with a solid background (the background color in exp settings),
// with a QGst::Ui::VideoWidget embedded in its center.
//
// HVideoWidget will be used in two situations. One is at full screen, where the
// size of the widget is fixed. A second is as a means to display stimuli in the
// experiment editor or in a window for testing experiment settings.
//
// HVideoWidget should be instantiated with the screen size intended to be used
// when running an experiment.
//
// In the second case the widget will maintain a fixed aspect ratio. The video widget
// will be scaled proportionally to the full-screen case.
//
// When HDisplayType is HDisplayTypeFullScreen and bMaintain is false, then the
// VideoWidget will simply fill the screen. If the widget is in a window, the VideoWidget
// will fill it completely. If bMaintain is true, then the stimulus
// is maximized in at least one dimension. Unless the a/r of the stimulus matches that
// of the screen, one of the dimensions will not fill the screen (and strips of the
// background color will be seen along the edges).
//
// When HDisplayType is HDisplayTypeOriginalSize, then the stimulus is shown (duh)
// at its original size scaled down by a factor of the current HVideoWidget size over
// its intended full-screen size.

class HStimulusWidget: public QFrame
{
	Q_OBJECT

	private:
		QSize m_sizeTargetScreen;
		HVideoWidget *m_videoWidget;


	public:
	    HStimulusWidget(const Habit::StimulusDisplayInfo& sdi, float screenWidth, float screenHeight, QWidget *parent = 0);
	    HStimulusWidget(bool bOriginalSize, bool bMaintainAspectRatio, unsigned int rgba, float screenWidth, float screenHeight, QWidget *parent = 0);

	    virtual ~HStimulusWidget();
	    //bool hasHeightForWidth() const { return true; }
	    //int heightForWidth(int w) const { return w * m_screenHeight / m_screenWidth; }
	    inline HVideoWidget *getHVideoWidget() const { return m_videoWidget; }

};



#endif /* LIBS_HABUTIL_HGST_HSTIMULUSWIDGET_H_ */
