/*
 * HResultsWidget.h
 *
 *  Created on: May 21, 2013
 *      Author: Oakeslab
 */

#ifndef HRESULTSWIDGET_H_
#define HRESULTSWIDGET_H_

#include <QTextEdit>
#include <QTextCharFormat>
#include "HResults.h"

class HResultsWidget : public QTextEdit
{
	Q_OBJECT

public:
	HResultsWidget(const HResults& results, QWidget* parent = NULL);
	virtual ~HResultsWidget() {};
	const HResults& results() const { return m_results; };

private:
	const HResults& m_results;
    QTextCharFormat m_textFormat;
    QTextCharFormat m_boldFormat;

    // Add header table at the cursor's current position
    void addHeaderTable(QTextCursor& cursor);

    // Add looks table at cursor's current position
    void addLooksTable(QTextCursor& cursor);

    // Add row to looks table
    void addTrialRow(QTextTable* table, const QStringList& s, QTextCharFormat textFormat, QTextCharFormat cellFormat);
};

#endif /* HRESULTSWIDGET_H_ */
