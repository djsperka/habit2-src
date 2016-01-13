/*
 * HStimulusInfoWidget.h
 *
 *  Created on: Oct 17, 2014
 *      Author: Oakeslab
 */

#ifndef HSTIMULUSINFOWIDGET_H_
#define HSTIMULUSINFOWIDGET_H_

#include "stimulusinfo.h"
#include <QWidget>
#include <QDir>

namespace Ui
{
	class HStimulusInfoForm;
};


namespace GUILib
{
	class HStimulusInfoWidget : public QWidget
	{
		Q_OBJECT

	public:
		HStimulusInfoWidget(const Habit::StimulusInfo& info, const QString& label, QWidget *parent=0, QDir dir = QDir());
		virtual ~HStimulusInfoWidget();

		// set m_info= info and initialize.
		void setStimulusInfo(const Habit::StimulusInfo& info);

		// get stimulus info currently set in the widget
		Habit::StimulusInfo getStimulusInfo();
	private:

		// establish connections
		void connections();

		// Make the widget reflect the contents of m_info
		void initialize();

		Ui::HStimulusInfoForm *ui;
		Habit::StimulusInfo m_info;
		QString m_label;
		QDir m_rootDir;

	signals:
		void stimulusInfoChanged();

	protected slots:
		void selectButtonClicked();
		void backgroundColorClicked();
		void customColorClicked();
		void backgroundColorChecked(bool);
	};
};




#endif /* HSTIMULUSINFOWIDGET_H_ */
