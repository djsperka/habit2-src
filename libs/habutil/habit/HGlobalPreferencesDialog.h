/*
 * HGlobalPreferencesDialog.h
 *
 *  Created on: Nov 18, 2014
 *      Author: Oakeslab
 */

#ifndef HGLOBALPREFERENCESDIALOG_H_
#define HGLOBALPREFERENCESDIALOG_H_

#include <QtGui>
#include "HTypes.h"

namespace GUILib
{
	class HGlobalPreferencesDialog: public QDialog
	{
		Q_OBJECT


		QPushButton *m_pbDone;
		QPushButton *m_pbCancel;
		QPushButton *m_pbSelect;
		QPushButton *m_pbSelectWorkspace;
		QPushButton *m_pbIdentify;
		QLineEdit *m_plineeditRoot;
		QLineEdit *m_plineeditWorkspace;
		QComboBox *m_pcbControl;
		QComboBox *m_pcbLeft;
		QComboBox *m_pcbCenter;
		QComboBox *m_pcbRight;

		void create();
		void connections();
		void initialize();
		int getNumberOfMonitors();

	signals:
		void workspaceChanged();

	protected slots:

		void doneClicked();
		void selectClicked();
		void selectWorkspaceClicked();
		void identifyClicked();

	public:
		HGlobalPreferencesDialog(QWidget *parent=NULL);
		virtual ~HGlobalPreferencesDialog() {};
		QString getRootFolder() const;
		void setRootFolder(const QString& s);
		int getMonitorID(const HPlayerPositionType& type) const;
		void setMonitorID(const HPlayerPositionType& type, int id);
	};


};



#endif /* HGLOBALPREFERENCESDIALOG_H_ */
