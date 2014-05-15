#ifndef TREEWIZARD_H
#define TREEWIZARD_H

#include <QtGui/QWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QComboBox>
#include <QtGui/QStackedWidget>

namespace GUILib
{
    class TreeWizard;
}

class GUILib::TreeWizard : public QWidget
{
    Q_OBJECT

public:
	static const QString NEW_EXPERIMENT;
	static const QString CLONE_EXPERIMENT;
    explicit TreeWizard(QWidget *parent = 0);
    ~TreeWizard();

public:
    void addPageItem(QWidget* page, const QString name, const QString parentNodeName);
    void addNodeItem(const QString name, const QString parentNodeName);
    void setCurrentPage(int index);
	QComboBox* getExperimentComboBox();
	void setControlsEnabled(bool isEnabled);
	void setControlsForViewOnly();

protected:
    QWidget* currentPage();
    void setCurrentIndex(int index);

signals:
    void done();
    void canceled();
	void experimentActivated(QString);

private slots:
    void update();
    void onPrevious();
    void onNext();
    void currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

private:
    QPushButton* nextBtn_;
    QPushButton* previousBtn_;
    QPushButton* cancelBtn_;
    QPushButton* doneBtn_;
    QTreeWidget* tree_;
	QComboBox* experimentBox_;
    QList<QWidget*> wizardPages_;
    QMap<int, QTreeWidgetItem*> pagesItemsMapping_;
    int currentPageIndex_;
    QStackedWidget* pageArea_;
    QFont nodeNameFont_;
    QFont pageNameFont_;
};

#endif // TREEWIZARD_H
