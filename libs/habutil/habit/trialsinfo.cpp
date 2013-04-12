#include "trialsinfo.h"

Habit::TrialsInfo::TrialsInfo() 
{
    type_ = Habit::TrialsInfo::eSubjectControlled;
    length_ = 200;
    lookTimes_ = 200;
    number_ = 3;
}

Habit::TrialsInfo::~TrialsInfo()
{

}

int Habit::TrialsInfo::getType() const
{
    return type_;
}

void Habit::TrialsInfo::setType(int type)
{
    type_ = type;
}

unsigned int Habit::TrialsInfo::getLength() const
{
    return length_;
}

void
Habit::TrialsInfo::setLength(unsigned length)
{
    length_ = length;
}

unsigned int 
Habit::TrialsInfo::getLookTimes() const
{
    return lookTimes_;
}

void
Habit::TrialsInfo::setLookTimes(unsigned int lookTimes)
{
    lookTimes_ = lookTimes;
}

unsigned int 
Habit::TrialsInfo::getNumberOfTrials() const
{
    return number_;
}

void
Habit::TrialsInfo::setNumberOfTrials(unsigned int number)
{
    number_ = number;
}