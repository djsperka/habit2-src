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
#include <QStringList>
#include <QList>
#include <QCheckBox>

class HGMM;
namespace GUILib {
	class HStimulusPreviewWidget;
}

class TestMMDialog: public QDialog
{
	Q_OBJECT

private:
	HGMM *m_pmm;
	GUILib::HStimulusPreviewWidget *m_previewWidget;
	QStringList m_stimNames;
	QList<unsigned int> m_stimIndices;
	QCheckBox *m_pFullScreen;
	QCheckBox *m_pSSWidget;	// use stimuli settings widget

public:
	TestMMDialog(QWidget *parent=0);
	~TestMMDialog();
	void components();

public slots:

	void experimentActivated(QString expt);
	void playItem(unsigned int);
	void stopItem();
	void stimStarted(int index);
	void agStarted(int index);
	void screen(int screenid, QString filename);
};


#endif /* APPS_TESTMM_TESTMMDIALOG_H_ */
