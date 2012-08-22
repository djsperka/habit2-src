/*
 *  HSingleton.h
 *  habutil
 *
 *  Created by Oakes Lab on 8/21/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 * Implementation of a singleton class for Habit. Taken from 
 * http://www.qtcentre.org/wiki/index.php?title=Singleton_pattern
 */

#ifndef _HSINGLETON_H_
#define _HSINGLETON_H_

#include <QObject>

template <class T>
class HSingleton
{
public:
    static T& instance()
    {
        static T _instance; // create static instance of our class
        return _instance;   // return it
    }
	
private:
    HSingleton();	// hide constructor
    ~HSingleton();	// hide destructor
    HSingleton(const HSingleton &); // hide copy constructor
    HSingleton& operator=(const HSingleton &); // hide assign op
};



#endif