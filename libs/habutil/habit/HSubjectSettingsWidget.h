/*
 * HSubjectSettingsWidget.h
 *
 *  Created on: Dec 2, 2014
 *      Author: Oakeslab
 */

#ifndef HSUBJECTSETTINGSWIDGET_H_
#define HSUBJECTSETTINGSWIDGET_H_

#include "stimulisettings.h"
#include "HStimulusOrderListModel.h"
#include <QWidget>

namespace Ui
{
	class HSubjectSettingsForm;
};


namespace GUILib
{
	class HSubjectSettingsWidget : public QWidget
	{
		Q_OBJECT

	public:
		HSubjectSettingsWidget(bool bTestRun, QWidget *parent=0);
		virtual ~HSubjectSettingsWidget();
		QString getComments() const;
		QString getObserverID() const;
		QString getSubjectID() const;
		bool isTestRun() const;

	private:

		Ui::HSubjectSettingsForm *ui;
		QString m_idSaved;
		QString m_obsSaved;
		QString m_notesSaved;

	signals:

	private slots:
		void testRunClicked(bool);
	};
};





#endif /* HSUBJECTSETTINGSWIDGET_H_ */
