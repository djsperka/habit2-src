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
#include <QDragEnterEvent>
#include <QDropEvent>

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
		HStimulusInfoWidget(const Habit::StimulusInfo& info, const QString& label, QWidget *parent=0, bool isVideoImage = true);
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
		bool m_bIsVideoImage;

	protected:
//		void dragEnterEvent(QDragEnterEvent *event);
//		void dropEvent(QDropEvent * event);

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
