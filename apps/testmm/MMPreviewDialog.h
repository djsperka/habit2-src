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
	HMediaManager *m_pmm;
	GUILib::HPStimulusSettingsListModel *m_pmodel;
	QListView *m_pListView;
	void populateMM(const Habit::ExperimentSettings& settings);
	void populateMM(const Habit::StimuliSettings& stimuli);

public:
	MMPreviewDialog(const Habit::StimulusDisplayInfo& info, const QDir& dirStimRoot = QDir(), QWidget *parent = NULL);
	~MMPreviewDialog() {};
	void preview(const Habit::ExperimentSettings& settings);
	void preview(const Habit::StimuliSettings& stimuli);
	void preview(const Habit::StimulusSettings& stimulus);
	void preview(const Habit::StimuliSettings& stimuli, const Habit::HStimulusOrder& order);

protected slots:

	void playClicked();
	void clearClicked();
	void onAGStarted();
	void onStimStarted(int);
	void onCleared();
	void itemActivated(const QModelIndex&);
};



#endif /* MMPREVIEWDIALOG_H_ */
