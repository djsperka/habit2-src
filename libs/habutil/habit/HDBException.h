/*
 * HDBException.h
 *
 *  Created on: May 16, 2016
 *      Author: dan
 */

#ifndef HDBEXCEPTION_H_
#define HDBEXCEPTION_H_

#include <stdexcept>
#include <string>
using namespace std;

namespace Habit
{
	class HDBException: public runtime_error
	{
	public:
		HDBException(const string& reason, const string& sql, const string& sqlerror);
		virtual ~HDBException() throw() {};
	};
}



#endif /* HDBEXCEPTION_H_ */
