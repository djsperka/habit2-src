/*
 * HGMMException.h
 *
 *  Created on: Nov 1, 2017
 *      Author: dan
 */

#ifndef LIBS_HABUTIL_HGST_HGMMEXCEPTION_H_
#define LIBS_HABUTIL_HGST_HGMMEXCEPTION_H_

#include <exception>
#include <string>
#include <sstream>

class HGMMException: public std::exception {
public:
	HGMMException(const std::string& reason);
	virtual ~HGMMException();
	virtual const char* what() const throw();
private:
	static std::ostringstream m_cnvt;
};

#endif /* LIBS_HABUTIL_HGST_HGMMEXCEPTION_H_ */
