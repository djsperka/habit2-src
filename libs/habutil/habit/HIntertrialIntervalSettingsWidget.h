/*
 * HIntertrialIntervalSettingsWidget.h
 *
 *  Created on: Apr 6, 2018
 *      Author: dan
 */

#ifndef LIBS_HABUTIL_HABIT_HINTERTRIALINTERVALSETTINGSWIDGET_H_
#define LIBS_HABUTIL_HABIT_HINTERTRIALINTERVALSETTINGSWIDGET_H_

#include <QWidget>
#include "attentiongettersettings.h"
#include "stimulusdisplayinfo.h"

namespace Ui
{
	class HIntertrialIntervalSettingsForm;
}

namespace GUILib {

class HIntertrialIntervalSettingsWidget: public QWidget
{
	Q_OBJECT

public:
	HIntertrialIntervalSettingsWidget(const Habit::AttentionGetterSettings& ags, const Habit::StimulusDisplayInfo& sdi, QWidget *parent=NULL);
	virtual ~HIntertrialIntervalSettingsWidget();
	Habit::AttentionGetterSettings getAttentionGetterSettings();
	const Habit::StimulusSettings& getCurrentStimulusSettings() { return m_ss; };
	bool isSoundOnlyAG();

private:
	Habit::AttentionGetterSettings m_agsOriginal;		// keep a copy of the original - treat as const, please
	Habit::StimulusDisplayInfo m_sdiOriginal;		// keep a copy of the original - treat as const, please
	Habit::StimulusSettings m_ss;						// init'd from original, then tracks latest-and-greatest
	Habit::StimulusSettings m_ssSaved;					// this is the Center/Left/Right saved when the Sound-only btn is checked, restored on uncheck
	Ui::HIntertrialIntervalSettingsForm *ui;

signals:
	void stimulusSettingsChanged();
private slots:
	void editButtonClicked();
	void rbSoundToggled(bool);
	void rbStimulusToggled(bool);
	void rbBackgroundToggled(bool);


};

} /* namespace GUILib */

#endif /* LIBS_HABUTIL_HABIT_HINTERTRIALINTERVALSETTINGSWIDGET_H_ */
