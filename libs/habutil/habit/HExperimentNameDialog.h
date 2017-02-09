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
#include "HExperimentListValidator.h"

namespace GUILib {

class HExperimentNameDialog: public QDialog
{
	Q_OBJECT
	HExperimentListValidator *m_pvalidator;
	QLineEdit * m_ple;
public:
	HExperimentNameDialog(const QStringList& list, const QString& defaultName = QString("Habit Experiment"), QWidget *parent = NULL);
	virtual ~HExperimentNameDialog();
	QString getNewValue();
};

} /* namespace GUILib */

#endif /* LIBS_HABUTIL_HABIT_HEXPERIMENTNAMEDIALOG_H_ */
