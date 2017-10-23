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
	HStimulusLayout(HVideoWidget *pVideoWidget, const HDisplayType& displayType, bool bMaintainAspectRatio, const QSize& sizeTargetScreen);
	~HStimulusLayout();

  QSize sizeHint() const;
  QSize minimumSize() const;
  int count() const;
  QLayoutItem *itemAt(int) const;
  QLayoutItem *takeAt(int);
  void setGeometry(const QRect &rect);

protected:
  void addItem(QLayoutItem *item) {}; // DO NOT CALL - NO-OP

private:
  QList<QLayoutItem*> m_list;
  HVideoWidget *m_pVideoWidget;
  const HDisplayType& m_displayType;
  bool m_bMaintainAspectRatio;		// only applies to display type HDisplayType::HDisplayTypeFullScreen
  QSize m_targetDisplaySize;			// only applies to HDisplayType::HDisplayTypeOriginalSize

};


#endif /* LIBS_HABUTIL_HGST_HSTIMULUSLAYOUT_H_ */
