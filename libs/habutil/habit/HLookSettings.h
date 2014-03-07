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
		HLookSettings(): m_id(-1), m_minLookTime(0), m_minLookAwayTime(0) {};
		HLookSettings(unsigned int minlook, unsigned int minlookaway): m_id(-1), m_minLookTime(minlook), m_minLookAwayTime(minlookaway) {};
		HLookSettings(const HLookSettings& ls): m_id(ls.getId()), m_minLookTime(ls.getMinLookTime()), m_minLookAwayTime(ls.getMinLookAwayTime()) {};
		HLookSettings& operator=(const HLookSettings& rhs)
		{
			if (this != &rhs)
			{
				m_id = rhs.getId();
				m_minLookTime = rhs.getMinLookTime();
				m_minLookAwayTime = rhs.getMinLookAwayTime();
			}
			return *this;
		};
		int getId() const { return m_id; };
		void setId(int id) { m_id = id; };
		unsigned int getMinLookTime() const { return m_minLookTime; };
		unsigned int getMinLookAwayTime() const { return m_minLookAwayTime; };
		void setMinLookTime(unsigned int minlook) { m_minLookTime = minlook; };
		void setMinLookAwayTime(unsigned int minlookaway) { m_minLookAwayTime = minlookaway; };

		void loadFromDB(int id);
		bool saveToDB(int id);

	private:
		int m_id;
		unsigned int m_minLookTime;
		unsigned int m_minLookAwayTime;
	};


	QDataStream & operator<< (QDataStream& stream, HLookSettings settings);
	QDataStream & operator>> (QDataStream& stream, HLookSettings& settings);
	bool operator==(const HLookSettings& lhs, const HLookSettings& rhs);
}; // namepsace

#endif /* HLOOKSETTINGS_H_ */
