
#ifndef HABIT_RUN_SETTINGS_H
#define HABIT_RUN_SETTINGS_H

#include <QtCore/QString>
#include <QtCore/QDataStream>

/// Common namespace for all entities of the Habit
namespace Habit
{

	class RunSettings
	{
	public:
		RunSettings();
		~RunSettings();

	public:
		int getId() const;
		void setId(int id);

		int getExperimentId() const;
		void setExperimentId(int id);

		QString getHabituationOrder() const;
		void setHabituationOrder(const QString&);
		bool isHabituationRandomized() const;
		void setHabituationRandomized(bool);
		int getHabituationRandomizeMethod() const;
		void setHabituationRandomizeMethod(int);

		QString getPretestOrder() const;
		void setPretestOrder(const QString&);
		bool isPretestRandomized() const;
		void setPretestRandomized(bool);
		int getPretestRandomizeMethod() const;
		void setPretestRandomizeMethod(int);

		QString getTestOrder() const;
		void setTestOrder(const QString&);
		bool isTestRandomized() const;
		void setTestRandomized(bool);
		int getTestRandomizeMethod() const;
		void setTestRandomizeMethod(int);

		int getSubjectId() const;
		void setSubjectId(int id);

	private:
		int id_;
		int experimentId_;
		int subjectId_;

		QString habituationTestOrder_;
		bool isHabituationOrderRandomized_;
		int habituationRandomizeMethod;

		QString pretestTestOrder_;
		bool isPretestOrderRandomized_;
		int pretestRandomizeMethod;

		QString testTestOrder_;
		bool isTestOrderRandomized_;
		int testRandomizeMethod;
	};

	QDataStream & operator << (QDataStream& stream, RunSettings settings);
	QDataStream & operator >> (QDataStream& stream, RunSettings& settings);


} // namespace Habit

#endif // HABIT_RUN_SETTINGS_H