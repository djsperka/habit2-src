/*
 * HOrderSettings.h
 *
 *  Created on: Apr 7, 2014
 *      Author: Oakeslab
 */

#ifndef HORDERSETTINGS_H_
#define HORDERSETTINGS_H_

#include "HTypes.h"
#include <QList>

namespace Habit
{

	class HOrderSettings
	{
	public:
		HOrderSettings() : m_bIsRandomized(false), m_pRandomizationType(&HRandomizationType::Simple) {};
		HOrderSettings(const HOrderSettings& order);
		~HOrderSettings() {};
		HOrderSettings& operator=(const HOrderSettings& rhs);

		// return list as a comma-separated string
		QString getCanonicalList() const;

		// clear current list and use the supplied list
		void setList(const QList<int>& list);

		// copy current list to the supplied list (append, don't clear).
		// return reference to 'list'
		void getList(QList<int>& list) const;

		bool getIsRandomized() const { return m_bIsRandomized; };
		void setIsRandomized(bool b) { m_bIsRandomized = b; };

		const HRandomizationType& getRandomizationType() const { return *m_pRandomizationType; };
		void setRandomizationType(const HRandomizationType& type) { m_pRandomizationType = &type; };

	private:

		QList<int> m_list;
		bool m_bIsRandomized;
		const HRandomizationType* m_pRandomizationType;
	};

	QDebug operator<<(QDebug dbg, const HOrderSettings& os);
//	QDataStream & operator<< (QDataStream& stream, HOrderSettings settings);
//	QDataStream & operator>> (QDataStream& stream, HOrderSettings& settings);
//	bool operator==(const OrderSettings& lhs, const HOrderSettings& rhs);


}; // namespace

#endif /* HORDERSETTINGS_H_ */
