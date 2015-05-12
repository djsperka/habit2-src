/*
 * HLookSettings.h
 *
 *  Created on: Feb 26, 2014
 *      Author: Oakeslab
 */

#ifndef HLOOKSETTINGS_H_
#define HLOOKSETTINGS_H_

#include <QDataStream>

namespace Habit
{
	class HLookSettings
	{
	public:
		HLookSettings(): m_id(-1), m_minLookTime(500), m_minLookAwayTime(100), m_bInclusiveLookTime(true) {};
		HLookSettings(unsigned int minlook, unsigned int minlookaway, bool bInclusiveLookTime = true): m_id(-1), m_minLookTime(minlook), m_minLookAwayTime(minlookaway), m_bInclusiveLookTime(bInclusiveLookTime) {};
		HLookSettings(const HLookSettings& ls): m_id(ls.getId()), m_minLookTime(ls.getMinLookTime()), m_minLookAwayTime(ls.getMinLookAwayTime()), m_bInclusiveLookTime(ls.getInclusiveLookTime()) {};
		HLookSettings& operator=(const HLookSettings& rhs)
		{
			if (this != &rhs)
			{
				m_id = rhs.getId();
				m_minLookTime = rhs.getMinLookTime();
				m_minLookAwayTime = rhs.getMinLookAwayTime();
				m_bInclusiveLookTime = rhs.getInclusiveLookTime();
			}
			return *this;
		};
		// A clone is a copy with db ID set to -1
		HLookSettings clone();
		int getId() const { return m_id; };
		void setId(int id) { m_id = id; };
		unsigned int getMinLookTime() const { return m_minLookTime; };
		unsigned int getMinLookAwayTime() const { return m_minLookAwayTime; };
		void setMinLookTime(unsigned int minlook) { m_minLookTime = minlook; };
		void setMinLookAwayTime(unsigned int minlookaway) { m_minLookAwayTime = minlookaway; };
		void setInclusiveLookTime(bool b) { m_bInclusiveLookTime = b; };
		bool getInclusiveLookTime() const { return m_bInclusiveLookTime; };

		void loadFromDB(int id);
		bool saveToDB(int id);

	private:
		int m_id;
		unsigned int m_minLookTime;
		unsigned int m_minLookAwayTime;
		bool m_bInclusiveLookTime;
	};


	QDataStream & operator<< (QDataStream& stream, HLookSettings settings);
	QDataStream & operator>> (QDataStream& stream, HLookSettings& settings);
	bool operator==(const HLookSettings& lhs, const HLookSettings& rhs);
}; // namepsace

#endif /* HLOOKSETTINGS_H_ */
