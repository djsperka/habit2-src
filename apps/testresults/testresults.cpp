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

    cursor.setPosition(topFrame->lastPosition());
    cursor.insertText("Experiment: ", textFormat);
    cursor.insertText(results->experimentSettings().getName(), boldFormat);
    cursor.insertBlock();
    cursor.insertText("Results file: ", textFormat);
    cursor.insertText(results->originalFilename(), boldFormat);
    cursor.insertBlock();
    cursor.insertText("Habit version:", textFormat);
    cursor.insertText(results->version(), boldFormat);
    cursor.insertBlock();
    cursor.insertText("Data collected on: ", textFormat);
    cursor.insertText("need date", boldFormat);
    cursor.insertHtml("<hr width=\"100%\"><br>");

    //! [4] //! [5]
    cursor.insertBlock();
    cursor.insertBlock();

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
			msgBox.setText("Cannot save data.");
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




