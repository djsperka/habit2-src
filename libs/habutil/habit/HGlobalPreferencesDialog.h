/*
 * HGlobalPreferencesDialog.h
 *
 *  Created on: Nov 18, 2014
 *      Author: Oakeslab
 */

#ifndef HGLOBALPREFERENCESDIALOG_H_
#define HGLOBALPREFERENCESDIALOG_H_

#include <QtGlobal>
#if QT_VERSION < 0x050000
#include <QtGui>
#else
#include <QtWidgets>
#endif
#include "HTypes.h"

namespace Ui
{
	class GlobalPreferencesDialog;
}

namespace GUILib
{
	class HGlobalPreferencesDialog: public QDialog
	{
		Q_OBJECT

		Ui::GlobalPreferencesDialog *ui;
		QString m_sWorkspaceSelected;	// this is what's displayed in the label; it is written to Settings when OK is clicked
		QString m_sStimulusRootDirSelected; // ditto, but for stimulus root dir
		void create();
		void connections();
		void initialize();
		int getNumberOfMonitors();

	signals:
		void workspaceChanged();

	protected slots:

		void accepted();
		void chooseStimulusRootDirClicked();
		void chooseWorkspaceClicked();
		void identifyClicked();
		void defaultStimRootClicked();

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
