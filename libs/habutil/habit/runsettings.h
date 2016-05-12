
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

	class PhaseRunSettings
	{
	private:

		QString m_orderName;			// order name or 'default'
		StimLabelList m_orderList;	// version 2
		bool m_isOrderRandomized;
		int m_randomizeMethod;
	public:
		PhaseRunSettings(): m_orderName(), m_isOrderRandomized(false), m_randomizeMethod(-1) {};
		virtual ~PhaseRunSettings() {};
		const QString& getOrderName() const { return m_orderName; };
		void setOrderName(const QString& name) { m_orderName = name; };
		const StimLabelList& getOrderList() const { return m_orderList; };
		void setOrderList(const StimLabelList& list) { m_orderList = list; };
		bool isOrderRandomized() const { return m_isOrderRandomized; };
		void setOrderRandomized(bool b) { m_isOrderRandomized = b; };
		int getRandomizeMethod() const { return m_randomizeMethod; };
		void setRandomizeMethod(int i) { m_randomizeMethod = i; };
	};
	QDataStream & operator << (QDataStream& stream, const PhaseRunSettings& prs);
	QDataStream & operator >> (QDataStream& stream, PhaseRunSettings& prs);
	bool operator==(const PhaseRunSettings& lhs, const PhaseRunSettings& rhs);

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

		int getSubjectId() const;
		void setSubjectId(int id);

		void insert(int seqno, const PhaseRunSettings& prs);

		void setMap(const QMap<int, PhaseRunSettings>& map) {phaseRunSettingsMap_ = map; };

		const QMap<int, PhaseRunSettings>& map() const { return phaseRunSettingsMap_; };
	private:
		int id_;
		int experimentId_;
		int subjectId_;

		QMap<int, PhaseRunSettings> phaseRunSettingsMap_;
	};

	QDataStream & operator << (QDataStream& stream, RunSettings settings);
	QDataStream & operator >> (QDataStream& stream, RunSettings& settings);
	bool operator==(const RunSettings& lhs, const RunSettings& rhs);


} // namespace Habit

#endif // HABIT_RUN_SETTINGS_H
