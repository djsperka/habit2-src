/*
 * HPreviewMediaManager.h
 *
 *  Created on: Aug 23, 2016
 *      Author: dan
 */

#ifndef HPREVIEWMEDIAMANAGER_H_
#define HPREVIEWMEDIAMANAGER_H_

#include "HMediaManager.h"
class QStackedWidget;

class HPreviewMediaManager : public HMediaManager
{
	Q_OBJECT
	const HStimulusLayoutType* m_pLayoutType;
	int m_currentLayoutPage;
	QStackedWidget *m_pstack;

public:
	HPreviewMediaManager(const HStimulusLayoutType& layoutType = HStimulusLayoutType::HStimulusLayoutSingle);
	virtual ~HPreviewMediaManager() {};
	void setLayoutType(const HStimulusLayoutType& type) { m_pLayoutType = &type; };
	virtual void stim(int istim);
	void addPlayers(HPlayer *p0, HPlayer *p1l, HPlayer *p1r, HPlayer *psound);
	QStackedWidget *stack() const { return m_pstack; }
};





#endif /* HPREVIEWMEDIAMANAGER_H_ */
