/*
 * HOrderSettingsWidget.h
 *
 *  Created on: Apr 7, 2014
 *      Author: Oakeslab
 */

#ifndef HORDERSETTINGSWIDGET_H_
#define HORDERSETTINGSWIDGET_H_

#include <QtGui>
#include <QRegExpValidator>
#include <QList>
#include "HOrderSettings.h"
//#include "habituationstimulitable.h"
#include "stimulisettings.h"

namespace GUILib
{
	// Validator which requires a list of integers between a min and max (inclusive).
	// May be separated by spaces, comma, pipe(s).
	class HStimOrderValidator: public QRegExpValidator
	{

		Q_OBJECT

	public:
		HStimOrderValidator(int max, int min=1)
		: QRegExpValidator(HStimOrderValidator::m_reFull)
		, m_min(min)
		, m_max(max)
		{};

		virtual ~HStimOrderValidator() {};

		virtual QValidator::State validate ( QString & input, int & pos ) const;

		void setMin(int min) { m_min = min; };
		void setMax(int max) { m_max = max; };

		// Check input string. If plist is supplied, it is filled with the parsed digits. Return true if
		// parse is OK and digits are within range.
		static bool getList(QString input, int min, int max, QList<int>* plist);

	private:
		static QRegExp m_reFull;
		static QRegExp m_reDelimDigit;
		int m_min;
		int m_max;
	};

	class HStimOrderLineEdit: public QLineEdit
	{

		Q_OBJECT

	public:
		HStimOrderLineEdit(int max, int min=1)
		: m_min(min)
		, m_max(max)
		{
			m_pValidator = new HStimOrderValidator(max, min);
			setValidator(m_pValidator);
		};
		virtual ~HStimOrderLineEdit() {};
		void getList(QList<int>& list) const;
		void setMin(int min) { m_min = min; m_pValidator->setMin(min); };
		void setMax(int max) { m_max = max; m_pValidator->setMax(max); };

	private:
		int m_min;
		int m_max;
		HStimOrderValidator *m_pValidator;
	};


	class HOrderSettingsWidget: public QWidget
	{

		Q_OBJECT

	public:
		HOrderSettingsWidget(const Habit::StimuliSettings& stimuli);
		~HOrderSettingsWidget() {};

		Habit::HOrderSettings getOrderSettings();
		void initialize(const Habit::StimuliSettings& stimuli);

	private:
		void createComponents();
		void makeConnections();
		void doLayout();
		void initialize();

		HabituationStimuliTable *m_pStimuliTable;
		HStimOrderLineEdit* m_plineeditOrder;
		QCheckBox *m_pcheckboxRandomize;
		QComboBox* m_pcomboRandomizationType;
		QLabel* m_plabelOrder;
		QLabel* m_plabelRandomizationType;
	};

};

#endif /* HORDERSETTINGSWIDGET_H_ */
