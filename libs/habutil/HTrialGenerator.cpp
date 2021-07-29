/*
 *  HTrialGenerator.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 9/19/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HTrialGenerator.h"
#include <QtDebug>
#include <QtGlobal>
#include <stdlib.h>

HTrialGenerator::HTrialGenerator(int N, bool bRand, bool bBlocks) : m_N(N), m_bRand(bRand), m_bBlocks(bBlocks)
{
	for (int i=0; i<m_N; i++) m_remaining.append(i);
	static bool bSeeded = false;
	if (!bSeeded)
	{
		srand(time(NULL));
		bSeeded = true;
	}
}

unsigned int HTrialGenerator::next()
{
	int val = -1;
	unsigned int irand;
	if (!m_bRand)
	{
		val = m_remaining.at(0);
		m_remaining.removeAt(0);
	}
	else 
	{
		if (m_bBlocks)
		{
			int irand;
			// draw random number between 0 and m_remaining.size()-1
			irand = rand() % m_remaining.size();
			val = m_remaining.at(irand);
			// remove element from m_remaining. 
			m_remaining.removeAt(irand);
		}
		else 
		{
			// draw random number between 0 and m_N. Do NOT remove element from m_remaining.
			irand = rand() % m_remaining.size();
			val = m_remaining.at(irand);
		}
	}
	
	// if m_remaining is empty, re-populate it.
	if (m_remaining.size() == 0)
		for (int i=0; i<m_N; i++) m_remaining.append(i);
	
	return val;
}
