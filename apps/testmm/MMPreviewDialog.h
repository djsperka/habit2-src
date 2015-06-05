/*
 * MMPreviewDialog.h
 *
 *  Created on: Jun 1, 2015
 *      Author: Oakeslab
 */

#ifndef MMPREVIEWDIALOG_H_
#define MMPREVIEWDIALOG_H_

#include <QDialog>
#include <QDir>
#include <QListView>
#include "experimentsettings.h"
#include "HMediaManager.h"
#include "HPStimulusSettingsListModel.h"

class MMPreviewDialog: public QDialog
{
	Q_OBJECT
	const Habit::ExperimentSettings& m_settings;
	HMediaManager *m_pmm;
	GUILib::HPStimulusSettingsListModel *m_pmodel;
	QListView *m_pListView;
	void populateMM();

public:
	MMPreviewDialog(const Habit::ExperimentSettings& settings, const QDir& dirStimRoot = QDir(), QWidget *parent = NULL);
	~MMPreviewDialog() {};

protected slots:

	void playClicked();
	void clearClicked();
	void onAGStarted();
	void onStimStarted(int);
	void onCleared();
	void itemActivated(const QModelIndex&);
};



#endif /* MMPREVIEWDIALOG_H_ */
