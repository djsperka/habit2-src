/*
 * TestMMDialog.h
 *
 *  Created on: Feb 22, 2017
 *      Author: dan
 */

#ifndef APPS_TESTMM_TESTMMDIALOG_H_
#define APPS_TESTMM_TESTMMDIALOG_H_

#include <QDialog>
#include <QWidget>
#include <QString>

class HMediaManager;

class TestMMDialog: public QDialog
{
	Q_OBJECT

private:
	HMediaManager *m_pmm;
	QStringList m_stimNames;
	QList<unsigned int> m_stimIndices;

public:
	TestMMDialog(QWidget *parent=0);
	~TestMMDialog();
	void components();

signals:
	void stim(int);

public slots:

	void experimentActivated(QString expt);
	void playItem(unsigned int);
	void stopItem();
	void stimStarted(int index);
	void screen(int screenid, QString filename);
};


#endif /* APPS_TESTMM_TESTMMDIALOG_H_ */
