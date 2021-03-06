/*
 * HLookSettings.cpp
 *
 *  Created on: Feb 26, 2014
 *      Author: Oakeslab
 */


#include "HLookSettings.h"
#include "maindao.h"
using namespace Habit;

QDataStream & Habit::operator<< (QDataStream& stream, HLookSettings settings)
{
	stream << settings.getId() << settings.getMinLookTime() << settings.getMinLookAwayTime();
	return stream;
}

QDataStream & Habit::operator>> (QDataStream& stream, HLookSettings& settings)
{
	int id;
	unsigned int l, la;
	stream >> id >> l >> la;
	settings.setId(id);
	settings.setMinLookTime(l);
	settings.setMinLookAwayTime(la);
	return stream;
}

bool Habit::operator==(const HLookSettings& lhs, const HLookSettings& rhs)
{
	return lhs.getId() == rhs.getId() && lhs.getMinLookTime() == rhs.getMinLookTime() && lhs.getMinLookAwayTime() == rhs.getMinLookAwayTime();
}

HLookSettings HLookSettings::clone()
{
	HLookSettings settings(*this);
	settings.setId(-1);
	return settings;
}

void HLookSettings::loadFromDB(int experimentID)
{
	Habit::MainDao maindao;
	maindao.getHLookSettingsForExperiment(experimentID, *this);
}

void HLookSettings::saveToDB(int experimentID)
{
	Habit::MainDao dao;
	dao.addOrUpdateHLookSettings(experimentID, *this);
}
