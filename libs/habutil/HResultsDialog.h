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
	void onOpen();
	void onExport();
	void onView();
	void onGenerate();
private:
	QString m_sExptName;
	QPushButton* m_pPrintButton;
	QPushButton* m_pOpenButton;
	QPushButton* m_pExportButton;
	QPushButton* m_pViewButton;
	QPushButton* m_pGenerateButton;
	HResultsWidget* m_pResultsWidget;
};


#endif /* HRESULTSDIALOG_H_ */
