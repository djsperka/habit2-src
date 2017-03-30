/*
 * HPlayer5.cpp
 *
 *  Created on: Feb 28, 2017
 *      Author: dan
 */

#include "HPlayer5.h"

HPlayer5::HPlayer5(int ID, QWidget* w, const QDir& dir)
: HPlayer(ID, w, dir)
{

}

HPlayer5::~HPlayer5() {
	// TODO Auto-generated destructor stub
}

unsigned int HPlayer5::addStimulusPrivate(unsigned int id)
{
	m_media.insert(id, new HMediaHandle(getStimulusInfo(id).getAbsoluteFileName(getStimulusRoot())));
	return id;
}

