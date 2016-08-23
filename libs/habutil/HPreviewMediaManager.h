/*
 * HPreviewMediaManager.h
 *
 *  Created on: Aug 23, 2016
 *      Author: dan
 */

#ifndef HPREVIEWMEDIAMANAGER_H_
#define HPREVIEWMEDIAMANAGER_H_

#include "HMediaManager.h"

class HPreviewMediaManager : public HMediaManager
{
	Q_OBJECT
	const HStimulusLayoutType* m_pLayoutType;
	int m_currentLayoutPage;

public:
	HPreviewMediaManager(const HStimulusLayoutType& layoutType = HStimulusLayoutType::HStimulusLayoutSingle);
	virtual ~HPreviewMediaManager() {};
	void setLayoutType(const HStimulusLayoutType& type) { m_pLayoutType = &type; };
	virtual void stim(int istim);
};





#endif /* HPREVIEWMEDIAMANAGER_H_ */
