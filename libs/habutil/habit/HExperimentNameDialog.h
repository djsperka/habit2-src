/*
 * ExperimentNameDialog.h
 *
 *  Created on: Jan 31, 2017
 *      Author: dan
 */

#ifndef LIBS_HABUTIL_HABIT_HEXPERIMENTNAMEDIALOG_H_
#define LIBS_HABUTIL_HABIT_HEXPERIMENTNAMEDIALOG_H_

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QDialogButtonBox>
#include "HExperimentListValidator.h"
#include "HExperimentListWidget.h"

namespace GUILib {

class HExperimentNameDialog: public QDialog
{
	Q_OBJECT
	HExperimentListValidator *m_pvalidator;
	QLineEdit * m_ple;
	HExperimentListWidget *m_plist;
	QCheckBox *m_pcheck;
	QDialogButtonBox *m_pbbox;

	protected slots:
	void textChanged(const QString& text);

public:
	HExperimentNameDialog(const QStringList& list, const QString& defaultName = QString("Habit Experiment"), QWidget *parent = NULL);
	virtual ~HExperimentNameDialog();
	QString getNewValue();
	bool isTemplateChosen();
	QString getTemplateChosen();
};

} /* namespace GUILib */

#endif /* LIBS_HABUTIL_HABIT_HEXPERIMENTNAMEDIALOG_H_ */
