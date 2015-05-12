/*
 * HExperimentListValidator.cpp
 *
 *  Created on: Nov 12, 2014
 *      Author: Oakeslab
 */

#include "HExperimentListValidator.h"

QValidator::State GUILib::HExperimentListValidator::validate(QString& input, int& pos) const
{
	QValidator::State state = QValidator::Invalid;
	if (input.isEmpty())
	{
		state = QValidator::Invalid;
	}
	else if (m_list.contains(input))
	{
		state = QValidator::Intermediate;
	}
	else
	{
		state = QValidator::Acceptable;
	}
	return state;
};




