/*
 * HExperimentStatusWidget.h
 *
 *  Created on: Jan 5, 2015
 *      Author: Oakeslab
 */

#ifndef HEXPERIMENTSTATUSWIDGET_H_
#define HEXPERIMENTSTATUSWIDGET_H_

#include "HTypes.h"
#include "ui_HExperimentStatusForm.h"
#include <QWidget>

#if 0
namespace Ui
{
	class HExperimentStatusForm;
};
#endif


namespace GUILib
{
	class HExperimentStatusWidget : public QWidget
	{
		Q_OBJECT

	public:
		HExperimentStatusWidget(const QString& name, bool bDisplayCurrentPhaseTrial, bool bDisplayLookingDirection, QWidget* parent);
		virtual ~HExperimentStatusWidget();

		void setExperimentName(QString name);
		void setStatus(QString status);
		void setPhase(QString phase);
		void setTrial(int trial, int repeat);
		void setLooking(QString sLookingAt);

		Ui::HExperimentStatusForm *ui;
		bool m_bDisplayCurrentPhaseTrial;
		bool m_bDisplayLookingDirection;

	private:
		// hide this?
		HExperimentStatusWidget();


		// establish connections
		void connections();

		// Make the widget reflect the contents of m_info
		void initialize(const QString& name);


	protected slots:
	};
};

#endif /* HEXPERIMENTSTATUSWIDGET_H_ */
