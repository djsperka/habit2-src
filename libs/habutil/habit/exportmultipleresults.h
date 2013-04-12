#ifndef EXPORTMULTIPLERESULTS_H
#define EXPORTMULTIPLERESULTS_H

#include <QtGui/QDialog>

class QLabel;
class QPushButton;
class QListWidget;
class QVBoxLayout;

namespace GUILib {

class ExportMultipleResults : public QDialog
{
	Q_OBJECT

public:
	ExportMultipleResults(QWidget* w = 0);
	~ExportMultipleResults();

	QStringList getFiles();

private slots:
	void onAddFiles();
	void onRemoveFiles();
	void onClearList();
	void onMoveUp();
	void onMoveDown();
	void onSelectionChanged();

private:
	void createComponents();
	void makeConnections();
	void doLayout();

private:
	QLabel* headerText_;
	QPushButton* addFilesButton_;
	QPushButton* removeFilesButton_;
	QPushButton* clearListButton_;
	QPushButton* moveUpButton_;
	QPushButton* moveDownButton_;

	QListWidget* list_;

	QVBoxLayout* mainLayout_;
	QPushButton* cancelButton_;
	QPushButton* exportButton_;
};

} //namespace GUILib

#endif // EXPORTMULTIPLERESULTS_H
