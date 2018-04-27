/*
 * HAboutHabitDialog.h
 *
 *  Created on: Apr 26, 2018
 *      Author: dan
 */

#ifndef LIBS_HABUTIL_HABIT_HABOUTHABITDIALOG_H_
#define LIBS_HABUTIL_HABIT_HABOUTHABITDIALOG_H_

#include  <QDialog>

namespace Ui
{
	class AboutHabitDialog;
}

namespace GUILib {

class HAboutHabitDialog: public QDialog
{
		Q_OBJECT

public:
	HAboutHabitDialog(QWidget *parent=NULL);
	virtual ~HAboutHabitDialog();

private:
	Ui::AboutHabitDialog *ui;
};

} /* namespace GUILib */

#endif /* LIBS_HABUTIL_HABIT_HABOUTHABITDIALOG_H_ */
