/*
 * HApplication.cpp
 *
 *  Created on: Jun 12, 2013
 *      Author: Oakeslab
 */

#include "HApplication.h"
#include "H2MainWindow.h"
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
		QWidget* pActiveWindow = activeWindow();

		// check if active window is the main window.
		// If so, then a file may be opened.
		GUILib::H2MainWindow* pMainWindow = dynamic_cast<GUILib::H2MainWindow*>(pActiveWindow);
		if (pMainWindow)
		{
			emit showResultsFile(static_cast<QFileOpenEvent *>(event)->file());
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





