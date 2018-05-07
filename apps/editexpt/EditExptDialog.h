/*
 *  EditExptDialog.h
 *  editexpt
 *
 *  Created by Oakes Lab on 9/12/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <QtGui>
#include <QDebug>
#include <QVector>
#include "experimentsettingsform.h"
#include "configuration.h"
#include "maindao.h"
#include "trialsinfo.h"
#include "HGMM.h"


class TestMMDialog: public QDialog
{
	Q_OBJECT
	
public:
	TestMMDialog(Habit::ExperimentSettings& es, QWidget* parent=0);

private slots:
	void valueChanged(int i);
signals:
	void stim(int);
	
private:
	QSpinBox* m_psb;
	HGMM* m_pmm;
};

class TestExptDialog: public QDialog
{
	Q_OBJECT
	
public:
	TestExptDialog(QWidget* parent=0);	
	~TestExptDialog() {};
	
private:
	QComboBox* m_cbExperiments;
	QPushButton* m_pbDump;
	QPushButton* m_pbTestMediaMgr;
	QVector<Habit::ExperimentSettings> m_vecExperiments;
	
	void dumpTrialsInfo(Habit::TrialsInfo info, const QString& sTrialsType);
	void dumpStimulusSettings(Habit::StimulusSettings settings, const QString& sText);
	void dumpStimulusInfo(Habit::StimulusInfo info, const QString& sText);
	void dumpStimuliSettings(Habit::StimuliSettings s, const QString& sText);
	
private slots:
	void onDump();
	void onTestMediaMgr();
};

class EditExptDialog: public QDialog
{
	Q_OBJECT
	
public:
	EditExptDialog(QWidget* parent=0);	
	~EditExptDialog() {};
	
protected:
	
private:
	QPushButton* m_pbEdit;
	QPushButton* m_pbTest;
	
	private slots:
	void onEdit();
	void onTest();
};

