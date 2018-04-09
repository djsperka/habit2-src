/*
 * HDBException.cpp
 *
 *  Created on: May 16, 2016
 *      Author: dan
 */

#include "HDBException.h"
#include <iostream>

Habit::HDBException::HDBException(const string& reason, const string& sql, const string& sqlerror)
: runtime_error(reason + '\n' + sql + '\n' + sqlerror)
{
}

