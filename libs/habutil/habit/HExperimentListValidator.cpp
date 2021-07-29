/*
 * HExperimentListValidator.cpp
 *
 *  Created on: Nov 12, 2014
 *      Author: Oakeslab
 */

#include "HExperimentListValidator.h"

QValidator::State GUILib::HExperimentListValidator::validate(QString& input, int&) const
{
	QValidator::State state = QValidator::Invalid;

	// if input is empty we're up in the air
	if (input.isEmpty())
	{
		state = QValidator::Intermediate;
	}
	else
	{
		// check against re
		QRegularExpressionMatch match = m_re.match(input);
		if (match.hasMatch())
		{
			// check if its in the list of existing experiments
			if (m_list.contains(input))
			{
				state = QValidator::Intermediate;
			}
			else
			{
				state = QValidator::Acceptable;
			}
		}
	}
	return state;
};




