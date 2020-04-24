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
#include "StimDisplayWidget.h"

namespace hmm { class HMM; };

class GstspControlDialog;
class GstspDialog: public QDialog
{
	Q_OBJECT

private:
	hmm::HMM *m_pmm;
	//QMap<hmm::HMMStimID, QString> m_mapStimIDNames;
	QList<hmm::HMMStimID> m_listIDs;		// list of stimid
	QStringList m_stringlistNames;			// list of stim names, same order as id list
	GstspControlDialog *m_control;
public:
	GstspDialog(QWidget *parent=0);
	~GstspDialog();
	void components();
	void setWidgetPropertyOnSinks(StimDisplayWidget *psdw, unsigned int nwidgets);

signals:
	void stim(unsigned int);

public slots:

	void preroll(QString);
	void dump();
	void experimentActivated(QString expt);
	void playItem(unsigned int);
	void playID(QString);
	void stopItem();
	void stimStarted(int index);
	void agStarted(int index);
	void screen(int screenid, QString filename);

};



#endif /* GSTSPDIALOG_H_ */
