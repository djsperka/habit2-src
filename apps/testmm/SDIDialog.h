/*
 * SDIDialog.h
 *
 *  Created on: May 20, 2015
 *      Author: Oakeslab
 */

#ifndef SDIDIALOG_H_
#define SDIDIALOG_H_

#include <QDialog>
#include "HStimulusDisplayInfoWidget.h"
class HMediaManager;

class SDIDialog : public QDialog
{
	Q_OBJECT
	HMediaManager *m_pmm;

public:
	SDIDialog(QWidget* w = 0);
	~SDIDialog();

private:
	GUILib::HStimulusDisplayInfoWidget *m_psdiw;

private slots:
	void showClicked();
	void playClicked();
	void clearClicked();
	void playerFinished(int);

signals:
};



#endif /* SDIDIALOG_H_ */
