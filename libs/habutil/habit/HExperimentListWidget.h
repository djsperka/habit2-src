/*
 * HExperimentListWidget.h
 *
 *  Created on: Sep 5, 2014
 *      Author: Oakeslab
 */

#ifndef HEXPERIMENTLISTWIDGET_H_
#define HEXPERIMENTLISTWIDGET_H_

#include <QListView>
#include <QString>
#include "HExperimentListModel.h"

namespace GUILib
{
	class HExperimentListWidget: public QListView
	{
		Q_OBJECT
		HExperimentListModel *m_pModel;
	public:
		HExperimentListWidget(QWidget *parent=0, bool bExp = true, bool bTemplates = false);
		virtual ~HExperimentListWidget();
		// get currently selected experiment; return empty string if no selection
		QString selectedExperiment();
		QStringList experimentList();
	signals:
		void experimentActivated(QString);

	private slots:
		void itemActivated(const QModelIndex&);

	public slots:
		void reload();

	};

};




#endif /* HEXPERIMENTLISTWIDGET_H_ */
