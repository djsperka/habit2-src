/*
 * StimDisplayWidget.h
 *
 *  Created on: Jan 14, 2020
 *      Author: dan
 */

#ifndef STIMDISPLAYWIDGET_H_
#define STIMDISPLAYWIDGET_H_

#include "stimulusdisplayinfo.h"
#include "HStimulusWidget.h"
#include "HMMTypes.h"
#include <vector>
#include <map>

class StimDisplayWidget: public QWidget
{
	std::vector<HVideoWidget *> m_vecVideoWidgets;
public:
	StimDisplayWidget(const Habit::StimulusDisplayInfo& sdi, unsigned long width, unsigned long height, const QWidget *parent=NULL);
	virtual ~StimDisplayWidget();
	HVideoWidget *getVideoWidget(int index);	// not nice, will return null
};

#endif /* STIMDISPLAYWIDGET_H_ */
