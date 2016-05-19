/*
 * HDBException.h
 *
 *  Created on: May 16, 2016
 *      Author: dan
 */

#ifndef HDBEXCEPTION_H_
#define HDBEXCEPTION_H_

#include <sstream>
#include <stdexcept>
#include <string>
using namespace std;

namespace Habit
{
	class HDBException: public exception
	{
	public:
		HDBException(const string& reason, const string& sql, const string& sqlerror);
		virtual ~HDBException() throw() {};
		virtual const char* what() const throw();
	private:
		static ostringstream m_cnvt;
	};
}



#endif /* HDBEXCEPTION_H_ */
