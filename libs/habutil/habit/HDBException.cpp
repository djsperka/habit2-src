/*
 * HDBException.cpp
 *
 *  Created on: May 16, 2016
 *      Author: dan
 */

#include "HDBException.h"

ostringstream Habit::HDBException::m_cnvt;

Habit::HDBException::HDBException(const string& reason, const string& sql, const string& sqlerror)
: exception()
{
	m_cnvt.str("");
	m_cnvt << reason << ":" << endl << "SQL=" << sql << endl << "SQL-error=" << sqlerror;
}

const char *Habit::HDBException::what() const throw()
{
	return m_cnvt.str().c_str();
}

