#include "HResultsUtil.h"
#include <QString>
#include <QFile>
#include <QDataStream>

bool saveExperimentResults(const QString& filename, const Habit::ExperimentSettings& expSettings, const Habit::RunSettings& runSettings, const Habit::SubjectSettings& subjectSettings, const HEventLog& log)
{
	bool b = false;
	QFile file(filename);
	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		QDataStream out(&file);
		out << expSettings << runSettings << subjectSettings << log;
		file.close();
		b = true;
	}
	return b;
}

bool loadExperimentResults(const QString& filename, Habit::ExperimentSettings& expSettings, Habit::RunSettings& runSettings, Habit::SubjectSettings& subjectSettings, HEventLog& log)
{
	bool b = false;
	QFile file(filename);
	if (file.open(QIODevice::ReadOnly))
	{
		QDataStream in(&file);
		in >> expSettings >> runSettings >> subjectSettings >> log;
		file.close();
		b = true;
	}
	return b;
}
