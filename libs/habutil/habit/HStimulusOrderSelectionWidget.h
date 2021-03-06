/*
 * HStimulusOrderSelectionWidget.h
 *
 *  Created on: Dec 1, 2014
 *      Author: Oakeslab
 */

#ifndef HSTIMULUSORDERSELECTIONWIDGET_H_
#define HSTIMULUSORDERSELECTIONWIDGET_H_

#include "stimulisettings.h"
#include "HStimulusOrderListModel.h"
#include "HTypes.h"
#include <QWidget>

namespace Ui
{
	class HStimulusOrderSelectionForm;
};


namespace GUILib
{
	class HStimulusOrderSelectionWidget : public QWidget
	{
		Q_OBJECT

	public:
		HStimulusOrderSelectionWidget(const Habit::StimuliSettings& s, const QString& phaseName, int seqno, QWidget *parent=0);
		virtual ~HStimulusOrderSelectionWidget();

		const HRandomizationType& getRandomizationType();
		bool isRandomized();
		bool isDefaultOrder();
		bool isHandOrder() { return false; };	// not implemented; intended for key-in order
		bool isDefinedOrder();
		QString getDefinedOrderName();
	private:

		Ui::HStimulusOrderSelectionForm *ui;
		GUILib::HStimulusOrderListModel* m_pmodel;
		Habit::HStimulusSettingsList m_ssList;
		Habit::HStimulusOrderList m_orderList;
		int m_seqno;							// this is used as key passed with orderChosen() signal
		void initialize(const QString& phaseName);
	signals:
		void orderChosen(int);

	private slots:
		void updateStatusLabel();

	};
};



#endif /* HSTIMULUSORDERSELECTIONWIDGET_H_ */
