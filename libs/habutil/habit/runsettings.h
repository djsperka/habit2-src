
#ifndef HABIT_RUN_SETTINGS_H
#define HABIT_RUN_SETTINGS_H

#include <QtCore/QString>
#include <QtCore/QDataStream>
#include <QList>
#include <QPair>

/// Common namespace for all entities of the Habit
namespace Habit
{

	typedef QList< QPair< int, QString> > StimLabelList;

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

		QString getHabituationOrderName() const;
		void setHabituationOrderName(const QString&);
		bool getHabituationOrderList(StimLabelList& list) const;
		void setHabituationOrderList(const StimLabelList& list);
		bool isHabituationRandomized() const;
		void setHabituationRandomized(bool);
		int getHabituationRandomizeMethod() const;
		void setHabituationRandomizeMethod(int);

		QString getPretestOrderName() const;
		void setPretestOrderName(const QString&);
		bool getPretestOrderList(StimLabelList& list) const;
		void setPretestOrderList(const StimLabelList& list);
		bool isPretestRandomized() const;
		void setPretestRandomized(bool);
		int getPretestRandomizeMethod() const;
		void setPretestRandomizeMethod(int);

		QString getTestOrderName() const;
		void setTestOrderName(const QString&);
		bool getTestOrderList(StimLabelList& list) const;
		void setTestOrderList(const StimLabelList& list);
		bool isTestRandomized() const;
		void setTestRandomized(bool);
		int getTestRandomizeMethod() const;
		void setTestRandomizeMethod(int);

		int getSubjectId() const;
		void setSubjectId(int id);

		// space separated or comma-separated string with int.
		// Returns true and list contains the parsed integers in order,
		// if each element parses as an int, false otherwise (and list
		// is undefined).
		static bool getOrderFromString(StimLabelList& list, QString str);


	private:
		int id_;
		int experimentId_;
		int subjectId_;

		QString habituationTestOrder_;			// order name or 'default'
		StimLabelList habituationOrderList_;	// version 2
		bool isHabituationOrderRandomized_;
		int habituationRandomizeMethod;

		QString pretestTestOrder_;			// order name or 'default'
		StimLabelList pretestOrderList_;	// version 2
		bool isPretestOrderRandomized_;
		int pretestRandomizeMethod;

		QString testTestOrder_;			// order name or 'default'
		StimLabelList testOrderList_;	// version 2
		bool isTestOrderRandomized_;
		int testRandomizeMethod;
	};

	QDataStream & operator << (QDataStream& stream, RunSettings settings);
	QDataStream & operator >> (QDataStream& stream, RunSettings& settings);
	bool operator==(const RunSettings& lhs, const RunSettings& rhs);


} // namespace Habit

#endif // HABIT_RUN_SETTINGS_H
