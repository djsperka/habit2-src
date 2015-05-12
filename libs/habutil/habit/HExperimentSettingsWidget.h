#ifndef EXPERIMENTSETTINGSWIDGET_H
#define EXPERIMENTSETTINGSWIDGET_H

#include "experimentsettings.h"

#include <QWidget>
#include <QMap>
#include <QListView>
#include <QStackedWidget>
#include <QPushButton>


namespace GUILib
{

	class HExperimentSettingsWidget : public QWidget
	{
		Q_OBJECT

	public:
		HExperimentSettingsWidget(const Habit::ExperimentSettings& experimentSettings, QWidget* w = 0, bool bViewOnly=false);
		~HExperimentSettingsWidget();

		// Returns the experiment settings as currently configured in the dialogs of this widget
		Habit::ExperimentSettings getExperimentSettings();

		// Assign the experiment settings that this widget uses. This method implies a call to initialize()
		void setExperimentSettings(const Habit::ExperimentSettings&);

		// synchronize the visible widgets/forms/etc to reflect what the underlying experiment settings hold.
		void initialize();

	private:
		void createComponents();
		void makeConnections();
		void doLayout();

	private slots:
		void cancelClicked();
		void saveClicked();
		void itemClicked(const QModelIndex& index);

	private:
		bool m_bViewOnly;	// if true, the ExperimentSettings in constructor are to be viewed, not saved, experiment combo not populated or active.
		Habit::ExperimentSettings m_experimentSettings;
		QMap<int, int> m_mapForms;	// first int is row in QListView, second is the page in the stacked widget

		QStackedWidget *m_pStackedWidget;		// contains the forms
		QListView *m_pListView;					// selection of forms
		QPushButton *m_pbtnSave;
		QPushButton *m_pbtnCancel;

	};
};	// namespace GUILib
#endif // HExperimentSettingsWidget_H
