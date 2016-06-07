/*
 * HExperimentListModel.cpp
 *
 *  Created on: Nov 12, 2014
 *      Author: Oakeslab
 */

#include "HExperimentListModel.h"
#include "maindao.h"
#include "experimentsettings.h"
#include "H2MainWindow.h"
#include <QtDebug>
#include <QMessageBox>
#include <QStringList>

GUILib::HExperimentListModel::HExperimentListModel()
{
	reload();
}

void GUILib::HExperimentListModel::reload()
{
	Habit::MainDao dao;
	setStringList(dao.getAllExperimentNames());
}

bool GUILib::HExperimentListModel::setData (const QModelIndex& index, const QVariant& value, int role)
{
	bool b = false;

	// get original experiment name
	QString exptOriginal = stringList().at(index.row());
	QString exptNew = value.toString();

	if (stringList().contains(exptNew))
	{
		QMessageBox::warning(NULL, "Change Experiment Name", "Duplicate experiment name!");
		b = false;
	}
	else
	{
		// Update database
		try
		{
			Habit::MainDao dao;
			dao.updateExperimentName(exptOriginal, exptNew);
			b = QStringListModel::setData(index, value, role);
		}
		catch (const Habit::HDBException& e)
		{
			QMessageBox::warning(NULL, "Change Experiment Name", QString("Cannot update experiment name: check log."));
		}
	}
	return b;
}

QVariant GUILib::HExperimentListModel::data(const QModelIndex & index, int role) const
{
	if (role != Qt::BackgroundRole) return QStringListModel::data(index, role);
	else
	{
		// Check experiment settings and set color accordingly
		Habit::ExperimentSettings settings;
		QStringList sProblems;
		try
		{
			settings.loadFromDB(stringList().at(index.row()));
		}
		catch (const Habit::HDBException& e)
		{
			qCritical() << "Cannot load experiment " << stringList().at(index.row()) << endl << e.what();
			return QBrush(Qt::red);
		}

		if (!H2MainWindow::checkExperimentSettings(settings, sProblems))
		{
			return QBrush(Qt::yellow);
		}
	}
	return QBrush(Qt::green);
}


