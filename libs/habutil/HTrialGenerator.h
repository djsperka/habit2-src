/*
 *  HTrialGenerator.h
 *  habutil
 *
 *  Created by Oakes Lab on 9/19/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _HTRIALGENERATOR_H_
#define _HTRIALGENERATOR_H_

#include <QList>


// Generate a list of indices that determine a sequence of trials. 
// Caller provides the number of trial types, N, and whether the 
// trials are to be randomized (and if so, how). Then, by calling 
// next() repeatedly the sequence of trial indices is generated. 
// Note the careful use of 'indices' here. The indices returned 
// will always be between 0 and N-1. I assume that the returned 
// index will be used as an index into an array or vector which 
// holds the actual trial information. 
class HTrialGenerator
{
public:
	
	// Create a trial generator for N trial types. The trial indices
	// returned will be randomized if bRand is true. If bRand is false, 
	// the indices returned by next() will be 0, 1, ..., N-1, 0, 1, 2, ....
	// If bRand is true and bBlocks is false, then each call to 
	// next() will return a random number between 0 and N-1. If bRand 
	// is true and bBlocks is true, then the same is true, but each 
	// value between 0 and N-1 will be returned once before any of them
	// is returned a second time. The same is true for each subsequent
	// "block" of indices. 
	
	HTrialGenerator(int N, bool bRand, bool bBlocks);
	~HTrialGenerator() {};
	
	// Get the next trial index.
	unsigned int next();

private:
	int m_N;
	bool m_bRand;
	bool m_bBlocks;
	QList<unsigned int> m_remaining;
};



#endif