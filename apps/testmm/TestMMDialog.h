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
#include <QCheckBox>

class HGMM;

class TestMMDialog: public QDialog
{
	Q_OBJECT

private:
	QString m_script;
	HGMM *m_pmm;
	QStringList m_stimNames;
	QList<unsigned int> m_stimIndices;
	QCheckBox *m_pFullScreen;
	QCheckBox *m_pSSWidget;	// use stimuli settings widget

public:
	TestMMDialog(const QString& script = QString(), QWidget *parent=0);
	~TestMMDialog();
	void components();

signals:
	void stim(unsigned int);

public slots:

	void experimentActivated(QString expt);
	void playItem(unsigned int);
	void stopItem();
	void stimStarted(int index);
	void screen(int screenid, QString filename);
};


#endif /* APPS_TESTMM_TESTMMDIALOG_H_ */
