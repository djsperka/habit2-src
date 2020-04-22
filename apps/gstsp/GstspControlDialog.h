/*
 * GstspControlDialog.h
 *
 *  Created on: Dec 18, 2019
 *      Author: dan
 */

#ifndef GSTSPCONTROLDIALOG_H_
#define GSTSPCONTROLDIALOG_H_


#include <QDialog>
#include <QStringList>
#include <QStringListModel>
#include <QAbstractItemView>
#include <QAbstractButton>
#include "HMMTypes.h"


namespace Ui {
class gstspcontroldialog;
}

class GstspControlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GstspControlDialog(const QStringList& list, const QList<hmm::HMMStimID>& idlist, QWidget *parent=0);
    ~GstspControlDialog();

signals:
	void preroll(QString id);
	void dump();
	void play(unsigned int);

private slots:
    void onPrerollClicked();
    void onDumpClicked();
    void onPlayClicked();

private:
    Ui::gstspcontroldialog *ui;
    QStringListModel *model;
    QList<hmm::HMMStimID> idList;
};




#endif /* GSTSPCONTROLDIALOG_H_ */
