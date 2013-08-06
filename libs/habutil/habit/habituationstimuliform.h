#ifndef HABITUATIONSTIMULIFORM_H
#define HABITUATIONSTIMULIFORM_H

#include "stimulisettings.h"
#ifndef USING_QT5
#include <QtGui>
#else
#include <QtWidgets>
#endif


namespace GUILib {

class HabituationStimuliTable;

class HabituationStimuliForm : public QWidget
{
	Q_OBJECT

public:
	HabituationStimuliForm(const Habit::StimuliSettings& settings, const QString& caption = "", QWidget *w = 0);
	~HabituationStimuliForm();
	
private:
	void createComponents();
	void setLabelsFont();
	void makeConnections();
	void doLayout();
	void doButtonsLayout();

public:
	Habit::StimuliSettings getConfigurationObject();
	void setConfigurationObject(const Habit::StimuliSettings&);
	void initialize();
	void setAllConfigObjects(const Habit::StimuliSettings& pretest, const Habit::StimuliSettings& habit, const Habit::StimuliSettings& test);
signals:
	void copyStimulusToPretest(Habit::StimulusSettings*);
	void copyStimulusToHabit(Habit::StimulusSettings*);
	void copyStimulusToTest(Habit::StimulusSettings*);
public slots:
	void onCopyStimulus(Habit::StimulusSettings*);
private slots:
	void onModifyStimulusClick();
	void onRemoveStimulusClick();
	void onNewStimulusClick();
	void onConfigurationChange(const Habit::StimulusSettings&);
	void onNewConfiguration(const Habit::StimulusSettings&);
	void onCopyStimulusClick();
	void onCopyStimulusPretestClick();
	void onCopyStimulusHabitClick();
	void onCopyStimulusTestClick();

private:
	QLabel* mainTitle_;
	QPushButton* modifyStimulus_;
	QPushButton* removeStimulus_;
	QPushButton* newStimulus_;
	QPushButton* copyStimulus_;
	QVBoxLayout* mainLayout_; 
	HabituationStimuliTable* stimuliSettingsTable_;
	Habit::StimuliSettings settings_;
	QString m_caption;
	QPushButton* copyStimulusPretest_;
	QPushButton* copyStimulusHabit_;
	QPushButton* copyStimulusTest_;
	HabituationStimuliTable* _pretestSettingsTable;
	HabituationStimuliTable* _habitSettingsTable;
	HabituationStimuliTable* _testSettingsTable;
};

}

#endif // HABITUATIONSTIMULIFORM_H
