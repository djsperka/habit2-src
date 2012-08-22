/*
 *  HElapsedTime.h
 *  habutil
 *
 *  Created by Oakes Lab on 8/21/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 * Implementation of a singleton class for Habit. Taken from 
 * http://www.qtcentre.org/wiki/index.php?title=Singleton_pattern
 *
 * This class provides consistent time information for various things in Habit. 
 * This is not really a singleton class, since it only has a single (static) method. 
 */


#ifndef _HSINGLETON_H_
#define _HSINGLETON_H_

#include <QElapsedTimer>

class HElapsedTimer
{
public:
	static int elapsed()
	{
		return (int)instance().elapsed();
	};
private:
	static bool m_isInitialized;
    static QElapsedTimer& instance()
    {
        static QElapsedTimer m_instance; // create static instance of our class
		if (!m_isInitialized)
		{
			m_instance.start();
			m_isInitialized = true;
		}
        return m_instance;   // return it
    }
    HElapsedTimer();	// hide constructor
    ~HElapsedTimer();	// hide destructor
    HElapsedTimer(const HElapsedTimer &); // hide copy constructor
    HElapsedTimer& operator=(const HElapsedTimer &); // hide assign op
};



#endif