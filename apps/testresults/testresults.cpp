#include <QApplication>
#include <QMessageBox>
#include "testresults.h"
#include "experimentsettings.h"
#include "runsettings.h"
#include "subjectsettings.h"
#include "HEventLog.h"
#include "HResults.h"

#include <QtGui>
//#include <QTextEdit>

QTextEdit* createSummary(const HResults* results)
{
	QTextEdit* editor = new QTextEdit;
    QTextCursor cursor(editor->textCursor());
    cursor.movePosition(QTextCursor::Start);

    QTextFrame *topFrame = cursor.currentFrame();
    QTextFrameFormat topFrameFormat = topFrame->frameFormat();
    topFrameFormat.setPadding(16);
    topFrame->setFrameFormat(topFrameFormat);

    QTextCharFormat textFormat;
    QTextCharFormat boldFormat;
    boldFormat.setFontWeight(QFont::Bold);

    QTextTableFormat tableFormat;
    tableFormat.setBackground(QColor("lightGray"));
    tableFormat.setBorder(0);
    QVector<QTextLength> constraints;
    constraints << QTextLength(QTextLength::PercentageLength, 25);
    constraints << QTextLength(QTextLength::PercentageLength, 75);
    tableFormat.setColumnWidthConstraints(constraints);

    cursor.setPosition(topFrame->lastPosition());
    QTextTable *table = cursor.insertTable(4, 2, tableFormat);
    //QTextTable *table = cursor.insertTable(4, 2);

    // First row, exp name
    QTextTableCell cell;
    cell = table->cellAt(0, 0);
    cursor = cell.firstCursorPosition();
    cursor.insertText("Experiment:", textFormat);

    cell = table->cellAt(0, 1);
    cursor = cell.firstCursorPosition();
    cursor.insertText(results->experimentSettings().getName(), boldFormat);

    cell = table->cellAt(1, 0);
    cursor = cell.firstCursorPosition();
    cursor.insertText("Habit version:", textFormat);

    cell = table->cellAt(1, 1);
    cursor = cell.firstCursorPosition();
    cursor.insertText(results->version(), boldFormat);

    cell = table->cellAt(2, 0);
    cursor = cell.firstCursorPosition();
    cursor.insertText("Results type:", textFormat);

    cell = table->cellAt(2, 1);
    cursor = cell.firstCursorPosition();
    cursor.insertText("Original run", boldFormat);

    cell = table->cellAt(3, 0);
    cursor = cell.firstCursorPosition();
    cursor.insertText("Results file:", textFormat);

    cell = table->cellAt(3, 1);
    cursor = cell.firstCursorPosition();
    cursor.insertText(results->originalFilename(), boldFormat);

    // move cursor to end, insert new frame
    cursor = editor->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.insertFrame(topFrameFormat);
    cursor.insertBlock();


    QTextTableFormat lookTableFormat;
    lookTableFormat.setBackground(QColor("lightGray"));
    lookTableFormat.setBorder(0);
    constraints.clear();
    constraints << QTextLength(QTextLength::PercentageLength, 20);
    constraints << QTextLength(QTextLength::PercentageLength, 10);
    constraints << QTextLength(QTextLength::PercentageLength, 10);
    constraints << QTextLength(QTextLength::PercentageLength, 20);
    constraints << QTextLength(QTextLength::PercentageLength, 10);
    constraints << QTextLength(QTextLength::PercentageLength, 30);
    lookTableFormat.setColumnWidthConstraints(constraints);

    table = cursor.insertTable(1, 6, lookTableFormat);
    cell = table->cellAt(0, 0);
    cursor = cell.firstCursorPosition();
    cursor.insertText("Phase", boldFormat);
    cell = table->cellAt(0, 1);
    cursor = cell.firstCursorPosition();
    cursor.insertText("Trial", boldFormat);
    cell = table->cellAt(0, 2);
    cursor = cell.firstCursorPosition();
    cursor.insertText("Repeat", boldFormat);
    cell = table->cellAt(0, 3);
    cursor = cell.firstCursorPosition();
    cursor.insertText("Direction", boldFormat);
    cell = table->cellAt(0, 4);
    cursor = cell.firstCursorPosition();
    cursor.insertText("Duration", boldFormat);
    cell = table->cellAt(0, 5);
    cursor = cell.firstCursorPosition();
    cursor.insertText("Notes", boldFormat);


    // iterate over event log.
    // When PhaseStart/PhaseEnd encountered, enter with a Note.
	QListIterator<HEvent*> events(results->eventLog());
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
			table->appendRows(1);
			cell = table->cellAt(table->rows()-1, 0);
			cursor = cell.firstCursorPosition();
			cursor.insertText(sPhase, textFormat);
			cell = table->cellAt(table->rows()-1, 5);
			cursor = cell.firstCursorPosition();
			cursor.insertText("Phase Start", textFormat);
		}
		else if (e->type() == HEventType::HEventPhaseEnd)
		{
			// append row, indicate end of phase
			table->appendRows(1);
			cell = table->cellAt(table->rows()-1, 0);
			cursor = cell.firstCursorPosition();
			cursor.insertText(sPhase, textFormat);
			cell = table->cellAt(table->rows()-1, 5);
			cursor = cell.firstCursorPosition();
			cursor.insertText("Phase End", textFormat);
			sPhase = "";
		}
		else if (e->type() == HEventType::HEventTrialStart)
		{
			HTrialStartEvent* ps = static_cast<HTrialStartEvent*>(e);
			iTrialNumber = ps->trialnumber();
			iRepeatNumber = ps->repeatnumber();
			table->appendRows(1);
			cell = table->cellAt(table->rows()-1, 0);
			cursor = cell.firstCursorPosition();
			cursor.insertText(sPhase, textFormat);
			cell = table->cellAt(table->rows()-1, 1);
			cursor = cell.firstCursorPosition();
			cursor.insertText(QString("%1").arg(iTrialNumber), textFormat);
			cell = table->cellAt(table->rows()-1, 2);
			cursor = cell.firstCursorPosition();
			cursor.insertText(QString("%1").arg(iRepeatNumber), textFormat);
			cell = table->cellAt(table->rows()-1, 5);
			cursor = cell.firstCursorPosition();
			cursor.insertText("Trial Start", textFormat);
		}
		else if (e->type() == HEventType::HEventTrialEnd)
		{
			HTrialEndEvent* ps = static_cast<HTrialEndEvent*>(e);
			table->appendRows(1);
			cell = table->cellAt(table->rows()-1, 0);
			cursor = cell.firstCursorPosition();
			cursor.insertText(sPhase, textFormat);
			cell = table->cellAt(table->rows()-1, 1);
			cursor = cell.firstCursorPosition();
			cursor.insertText(QString("%1").arg(iTrialNumber), textFormat);
			cell = table->cellAt(table->rows()-1, 2);
			cursor = cell.firstCursorPosition();
			cursor.insertText(QString("%1").arg(iRepeatNumber), textFormat);
			cell = table->cellAt(table->rows()-1, 5);
			cursor = cell.firstCursorPosition();
			cursor.insertText(QString("Trial End (%1)").arg(ps->endtype().name()), textFormat);
		}
		else if (e->type() == HEventType::HEventLook)
		{
			HLookEvent* ps = static_cast<HLookEvent*>(e);
			table->appendRows(1);
			cell = table->cellAt(table->rows()-1, 0);
			cursor = cell.firstCursorPosition();
			cursor.insertText(sPhase, textFormat);
			cell = table->cellAt(table->rows()-1, 1);
			cursor = cell.firstCursorPosition();
			cursor.insertText(QString("%1").arg(iTrialNumber), textFormat);
			cell = table->cellAt(table->rows()-1, 2);
			cursor = cell.firstCursorPosition();
			cursor.insertText(QString("%1").arg(iRepeatNumber), textFormat);
			cell = table->cellAt(table->rows()-1, 3);
			cursor = cell.firstCursorPosition();
			cursor.insertText("TODO", textFormat);
			cell = table->cellAt(table->rows()-1, 4);
			cursor = cell.firstCursorPosition();
			cursor.insertText(QString("%1").arg(ps->look().lookMS()), textFormat);
			cell = table->cellAt(table->rows()-1, 5);
			cursor = cell.firstCursorPosition();
			cursor.insertText("look", textFormat);
		}
	}

    editor->setReadOnly(true);
    return editor;
}



int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	app.setApplicationName("testresults");
	app.setOrganizationName("Infant Cognition Lab");
	app.setOrganizationDomain("infantcognitionlab.ucdavis.edu");

	QString filename = QFileDialog::getOpenFileName(NULL, "Open File", "~/Desktop", "Habit Results Files (*.hab)");
	if (!filename.isNull())
	{
		HResults* results = HResults::load(filename);
		if (!results)
		{
			QMessageBox msgBox;
			msgBox.setText("Cannot load data.");
			msgBox.exec();
		}
		else
		{
			// Got result. Generate summary.
			QDialog dialog;
			QVBoxLayout* layout = new QVBoxLayout;
			layout->addWidget(createSummary(results));
			dialog.setLayout(layout);
			dialog.exec();
			delete results;
		}
	}

	return 0;
}




