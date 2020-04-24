/*
 * HStimulusLayout.h
 *
 *  Created on: Oct 6, 2017
 *      Author: dan
 */

#ifndef LIBS_HABUTIL_HGST_HSTIMULUSLAYOUT_H_
#define LIBS_HABUTIL_HGST_HSTIMULUSLAYOUT_H_


#include <QtWidgets>
#include <QList>
#include <QSize>
#include "HTypes.h"
#include "HVideoWidget.h"

class HStimulusLayout : public QLayout
{
	Q_OBJECT

public:
	HStimulusLayout(HVideoWidget *pVideoWidget, bool bOriginalSize, bool bMaintainAspectRatio, const QSize& sizeTargetScreen);
	~HStimulusLayout();

  QSize sizeHint() const;
  QSize minimumSize() const;
  int count() const;
  QLayoutItem *itemAt(int) const;
  QLayoutItem *takeAt(int);
  void setGeometry(const QRect &rect);
  void addItem(QLayoutItem *item);

private:
  QLayoutItem* m_item;	// only one item is allowed, and it must be an HVideoWidget
  bool m_bOriginalSize;				// maintain original size of stim
  bool m_bMaintainAspectRatio;		// only matters when false==m_bOriginalSize
  QSize m_targetDisplaySize;		// only matters when true==m_bOiginalSize

};


#endif /* LIBS_HABUTIL_HGST_HSTIMULUSLAYOUT_H_ */
