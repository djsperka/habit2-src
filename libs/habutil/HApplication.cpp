/*
 * HApplication.cpp
 *
 *  Created on: Jun 12, 2013
 *      Author: Oakeslab
 */

#include "HApplication.h"
#include "HMainWindow.h"
#include <QEvent>
#include <QtDebug>

bool HApplication::event(QEvent* event)
{
	bool bstatus = false;	// true if event recognized and processed
	switch (event->type())
	{
	case QEvent::FileOpen:
	{
		QWidget* pActiveModal = activeModalWidget();
		QWidget* pActivePopup = activePopupWidget();
		QWidget* pActiveWindow = activeWindow();

		// check if active window is the main window
		HMainWindow* pMainWindow = dynamic_cast<HMainWindow*>(pActiveWindow);
		if (pMainWindow)
		{
			qDebug() << "Got FileOpen event! Active window = main window";
		}
		else
		{
			qDebug() << "Got FileOpen event! Active window unknown";
		}

		bstatus = true;
		break;
	}
	default:
		bstatus = QApplication::event(event);
		break;
	}
	return bstatus;
}





