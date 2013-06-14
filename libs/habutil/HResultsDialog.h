/*
 * HResultsDialog.h
 *
 *  Created on: May 28, 2013
 *      Author: Oakeslab
 */

#ifndef HRESULTSDIALOG_H_
#define HRESULTSDIALOG_H_

#include <QtGui>
#include "HResultsWidget.h"

class HResultsDialog : public QDialog
{
	Q_OBJECT

public:
	HResultsDialog(const HResults& results, QWidget* parent = NULL);
	virtual ~HResultsDialog() {};

private slots:
	void onPrint();
	void onExport();
private:
	QPushButton* m_pPrintButton;
	QPushButton* m_pExportButton;
	HResultsWidget* m_pResultsWidget;
};


#endif /* HRESULTSDIALOG_H_ */
