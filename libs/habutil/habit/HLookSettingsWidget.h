/*
 * HLookSettingsWidget.h
 *
 *  Created on: Feb 26, 2014
 *      Author: Oakeslab
 */

#ifndef HLOOKSETTINGSWIDGET_H_
#define HLOOKSETTINGSWIDGET_H_

#include <QtGui>
#include "HLookSettings.h"

namespace Ui
{
	class HLookSettingsForm;
}

namespace GUILib
{
	class HLookSettingsWidget: public QWidget
	{
		Q_OBJECT

	public:
		HLookSettingsWidget();
		HLookSettingsWidget(const Habit::HLookSettings& settings, QWidget* parent=0);
		~HLookSettingsWidget();
		const Habit::HLookSettings& getHLookSettings();
		void setHLookSettings(const Habit::HLookSettings& settings);

	private:

		Ui::HLookSettingsForm *ui;
		Habit::HLookSettings m_settings;

		// Set values in widget to match those in m_settings
		void initialize();

		// Set up validators for gui components
		void setValidators();

	};
}





#if 0

namespace GUILib
{
	class HLookSettingsWidget: public QWidget
	{
		Q_OBJECT

	public:
		HLookSettingsWidget();
		HLookSettingsWidget(const Habit::HLookSettings& settings);
		const Habit::HLookSettings& getHLookSettings();
		void setHLookSettings(const Habit::HLookSettings& settings);

	private:
		// Set values in widget to match those in m_settings
		void initialize();

		// Create gui components
		void createComponents();

		// Set up validators for gui components
		void setValidators();

		// Establish connections for gui components
		void makeConnections();

		// Create layouts
		void doLayout();

		Habit::HLookSettings m_settings;


		QGroupBox* m_pgb;
		QGroupBox* m_pgbLook;
		QGroupBox* m_pgbLookAway;
		QLineEdit* m_peditLook;
		QLineEdit* m_peditLookAway;
		QCheckBox* m_pcbInclusiveLookTime;
	};
}; // namespace
#endif



#endif /* HLOOKSETTINGSWIDGET_H_ */
