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

	QStringListModel *m_pmodel;
	QPushButton *m_playButton;
	QPushButton *m_stopButton;
	QListView *m_list;

public:
	TestMMController(const QStringList& names, QWidget *parent=0);
	~TestMMController() {};

signals:
	void playItem(unsigned int);
	void stopItem();

public slots:

	void playClicked();
	void stopClicked();

};




#endif /* APPS_TESTMM_TESTMMCONTROLLER_H_ */