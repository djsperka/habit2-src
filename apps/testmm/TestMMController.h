/*
 * TestMMController.h
 *
 *  Created on: Feb 22, 2017
 *      Author: dan
 */

#ifndef APPS_TESTMM_TESTMMCONTROLLER_H_
#define APPS_TESTMM_TESTMMCONTROLLER_H_

#include <QDialog>
#include <QWidget>
#include <QStringList>
#include <QStringListModel>

class QPushButton;
class QListView;

class TestMMController: public QDialog
{
	Q_OBJECT

	QPushButton *m_playButton;
	QPushButton *m_stopButton;
	QPushButton *m_nextButton;
	QPushButton *m_replayButton;
	QListView *m_list;
	QStringList m_stringList;
	unsigned int m_index;

public:
	TestMMController(const QStringList& names, QWidget *screens=0, QWidget *parent=0);
	virtual ~TestMMController() {};

signals:
	void playItem(unsigned int);
	void stopItem();

public slots:

	void playClicked();
	void stopClicked();
	void nextClicked();
	void replayClicked();

};




#endif /* APPS_TESTMM_TESTMMCONTROLLER_H_ */
