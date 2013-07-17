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
#include <QFileOpenEvent>

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

		// check if active window is the main window.
		// If so, then a file may be opened.
		HMainWindow* pMainWindow = dynamic_cast<HMainWindow*>(pActiveWindow);
		if (pMainWindow)
		{
			qDebug() << "Got FileOpen event! Active window = main window";
			emit showResultsFile(static_cast<QFileOpenEvent *>(event)->file());
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




