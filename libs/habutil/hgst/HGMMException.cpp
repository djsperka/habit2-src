/*
 * HGMMException.cpp
 *
 *  Created on: Nov 1, 2017
 *      Author: dan
 */

#include <hgst/HGMMException.h>
std::ostringstream HGMMException::m_cnvt;

HGMMException::HGMMException(const std::string& reason)
{
	m_cnvt.str("");
	m_cnvt << reason;
}

HGMMException::~HGMMException()
{
	// TODO Auto-generated destructor stub
}


const char *HGMMException::HGMMException::what() const throw()
{
	return m_cnvt.str().c_str();
}
