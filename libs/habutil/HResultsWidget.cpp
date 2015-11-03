/*
 * HResultsWidget.cpp
 *
 *  Created on: May 21, 2013
 *      Author: Oakeslab
 */

#include "HResultsWidget.h"
#include <QtGui>

HResultsWidget::HResultsWidget(const HResults& results, QWidget* parent)
: QTextEdit(parent)
, m_results(results)
{
    QTextCursor cursor(this->textCursor());
    QTextFrame *topFrame = cursor.currentFrame();

    // Set padding for top frame?
    QTextFrameFormat topFrameFormat = topFrame->frameFormat();
    //topFrameFormat.setPadding(16);
    topFrame->setFrameFormat(topFrameFormat);

    // Set up regular and bold formats for text
    m_textFormat.setFontPointSize(10);
    m_boldFormat.setFontPointSize(10);
    m_boldFormat.setFontWeight(QFont::Bold);

    // top/header table
    cursor.setPosition(topFrame->lastPosition());
    addHeaderTable(cursor);


    // move cursor to end, insert new frame
    cursor.movePosition(QTextCursor::End);
    cursor.insertFrame(topFrameFormat);
    cursor.insertBlock();

    // Now looks table
    addLooksTable(cursor);


    // set editor readonly.
    setReadOnly(true);

    // set min width to 300 pixels.
    setMinimumWidth(600);
}

void HResultsWidget::addLooksTable(QTextCursor& cursor)
{
    QColor trialColors[2] =
    {
    		QColor("lightGray"),
    		QColor("darkGray")
    };
    int trialCounter = 0;
    QTextCharFormat cellFormat;
    QTextTableFormat lookTableFormat;
    QVector<QTextLength> constraints;

    // set up table format and column size constraints
    lookTableFormat.setBackground(QColor("white"));
    lookTableFormat.setBorder(0);
    constraints.clear();
    constraints << QTextLength(QTextLength::PercentageLength, 20);
    constraints << QTextLength(QTextLength::PercentageLength, 10);
    constraints << QTextLength(QTextLength::PercentageLength, 10);
    constraints << QTextLength(QTextLength::PercentageLength, 20);
    constraints << QTextLength(QTextLength::PercentageLength, 10);
    constraints << QTextLength(QTextLength::PercentageLength, 30);
    lookTableFormat.setColumnWidthConstraints(constraints);

    QTextTable* table = cursor.insertTable(1, 6, lookTableFormat);

    QStringList slist;
    slist << "Phase" << "Trial" << "Repeat" << "Direction" << "Duration" << "Notes";
    addTrialRow(table, slist, m_boldFormat, QTextCharFormat());

    // iterate over event log.
    // When PhaseStart/PhaseEnd encountered, enter with a Note.
	QListIterator<HEvent*> events(m_results.eventLog());
	events.toFront();

	QString sPhase("");
	int iTrialNumber = 0;
	int iRepeatNumber = -1;
	while (events.hasNext())
	{
		HEvent* e = events.next();
		if (e->type() == HEventType::HEventPhaseStart)
		{
			HPhaseStartEvent* ps = static_cast<HPhaseStartEvent*>(e);
			sPhase = ps->phase();

			// append row, indicate start of phase

			slist.clear();
			slist << sPhase << "" << "" << "" << "" << "Phase Start";
			addTrialRow(table, slist, m_textFormat, QTextCharFormat());
		}
		else if (e->type() == HEventType::HEventPhaseEnd)
		{
			// append row, indicate end of phase
			slist.clear();
			slist << sPhase << "" << "" << "" << "" << "Phase End";
			addTrialRow(table, slist, m_textFormat, QTextCharFormat());
			sPhase = "";
		}
		else if (e->type() == HEventType::HEventTrialStart)
		{
			HTrialStartEvent* ps = static_cast<HTrialStartEvent*>(e);
			iTrialNumber = ps->trialnumber() + 1;	// DJS 11-2-2015 Use trial number (not index) in output
			iRepeatNumber = ps->repeatnumber();
			cellFormat.setBackground(trialColors[trialCounter % 2]);

			slist.clear();
			slist << sPhase << QString("%1").arg(iTrialNumber) << QString("%1").arg(iRepeatNumber) << "" << "" << "Trial Start";
			addTrialRow(table, slist, m_textFormat, cellFormat);
		}
		else if (e->type() == HEventType::HEventTrialEnd)
		{
			HTrialEndEvent* ps = static_cast<HTrialEndEvent*>(e);
			slist.clear();
			slist << sPhase << QString("%1").arg(iTrialNumber) << QString("%1").arg(iRepeatNumber) << "" << "" << QString("Trial End (%1)").arg(ps->endtype().name());
			addTrialRow(table, slist, m_textFormat, cellFormat);
			trialCounter++;
		}
		else if (e->type() == HEventType::HEventLook)
		{
			HLookEvent* ps = static_cast<HLookEvent*>(e);

			slist.clear();
			slist << sPhase << QString("%1").arg(iTrialNumber) << QString("%1").arg(iRepeatNumber) << ps->look().direction().name() << QString("%1").arg(ps->look().lookMS()) << "look";
			addTrialRow(table, slist, m_textFormat, cellFormat);
		}
		else if (e->type() == HEventType::HHabituationFailure)
		{
			slist.clear();
			slist << sPhase << "" << "" << "" << "" << "Habituation failed";
			addTrialRow(table, slist, m_textFormat, cellFormat);
		}
		else if (e->type() == HEventType::HHabituationSuccess)
		{
			slist.clear();
			slist << sPhase << "" << "" << "" << "" << "Habituation success";
			addTrialRow(table, slist, m_textFormat, cellFormat);
		}
	}
	return;
}


void HResultsWidget::addTrialRow(QTextTable* table, const QStringList& s, QTextCharFormat textFormat, QTextCharFormat cellFormat)
{
	QTextCursor cursor;
    QTextTableCell cell;
    int i;
	table->appendRows(1);

	for (i=0; i<6; i++)
	{
		cell = table->cellAt(table->rows()-1, i);
		cell.setFormat(cellFormat);
		cursor = cell.firstCursorPosition();
		cursor.insertText(s[i], textFormat);
	}

	return;
}



void HResultsWidget::addHeaderTable(QTextCursor& cursor)
{
    // table format
    QTextTableFormat tableFormat;
    tableFormat.setBackground(QColor("lightGray"));
    tableFormat.setBorder(0);
    QVector<QTextLength> constraints;
    constraints << QTextLength(QTextLength::PercentageLength, 25);
    constraints << QTextLength(QTextLength::PercentageLength, 75);
    tableFormat.setColumnWidthConstraints(constraints);

    QTextTable *table = cursor.insertTable(4, 2, tableFormat);

    // First row, exp name
    QTextTableCell cell;
    cell = table->cellAt(0, 0);
    cursor = cell.firstCursorPosition();
    cursor.insertText("Experiment:", m_textFormat);

    cell = table->cellAt(0, 1);
    cursor = cell.firstCursorPosition();
    cursor.insertText(m_results.experimentSettings().getName(), m_boldFormat);

    cell = table->cellAt(1, 0);
    cursor = cell.firstCursorPosition();
    cursor.insertText("Habit version:", m_textFormat);

    cell = table->cellAt(1, 1);
    cursor = cell.firstCursorPosition();
    cursor.insertText(m_results.version(), m_boldFormat);

    cell = table->cellAt(2, 0);
    cursor = cell.firstCursorPosition();
    cursor.insertText("Results type:", m_textFormat);

    cell = table->cellAt(2, 1);
    cursor = cell.firstCursorPosition();
    cursor.insertText("Original run", m_boldFormat);

    cell = table->cellAt(3, 0);
    cursor = cell.firstCursorPosition();
    cursor.insertText("Results file:", m_textFormat);

    cell = table->cellAt(3, 1);
    cursor = cell.firstCursorPosition();
    cursor.insertText(m_results.originalFilename(), m_boldFormat);
}

