/*
 * GstspDialog.h
 *
 *  Created on: Nov 18, 2019
 *      Author: dan
 */

#ifndef GSTSPDIALOG_H_
#define GSTSPDIALOG_H_

#include <QDialog>
#include <QMap>
#include <QString>
#include "HMMTypes.h"

namespace hmm { class HMM; };

class GstspDialog: public QDialog
{
	Q_OBJECT

private:
	hmm::HMM *m_pmm;
	QMap<hmm::HMMStimID, QString> m_mapStimIDNames;

public:
	GstspDialog(QWidget *parent=0);
	~GstspDialog();
	void components();

signals:
	void stim(unsigned int);

public slots:

	void experimentActivated(QString expt);
	void playItem(unsigned int);
	void stopItem();
	void stimStarted(int index);
	void agStarted(int index);
	void screen(int screenid, QString filename);

};



#endif /* GSTSPDIALOG_H_ */
