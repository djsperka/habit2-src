#include "trialsinfo.h"

Habit::TrialsInfo::TrialsInfo(const HTrialCompletionType& ctype)
: pctype_(&ctype)
 , length_(200)
 , lookTimes_(200)
 , number_(3)
{
}

Habit::TrialsInfo::~TrialsInfo()
{

}

QDataStream & Habit::operator<< (QDataStream& stream, Habit::TrialsInfo info)
{
	stream << info.getTrialCompletionType().number() << info.getLength() << info.getLookTimes() << info.getNumberOfTrials();
	return stream;
}

QDataStream & Habit::operator>> (QDataStream& stream, Habit::TrialsInfo& info)
{
	int i;
	unsigned int length, times, number;
	stream >> i >> length >> times >> number;
	info.setTrialCompletionType(getTrialCompletionType(i));
	info.setLength(length);
	info.setLookTimes(times);
	info.setNumberOfTrials(number);
	return stream;
}

bool Habit::operator==(const Habit::TrialsInfo& lhs, const Habit::TrialsInfo& rhs)
{
	return (lhs.getTrialCompletionType() == rhs.getTrialCompletionType() &&
			lhs.getLength() == rhs.getLength() &&
			lhs.getLookTimes() == rhs.getLookTimes() &&
			lhs.getNumberOfTrials() == rhs.getNumberOfTrials());
}




const HTrialCompletionType& Habit::TrialsInfo::getTrialCompletionType() const
{
	return *pctype_;
}

void Habit::TrialsInfo::setTrialCompletionType(const HTrialCompletionType& ctype)
{
	pctype_ = &ctype;
}

#if 0
int Habit::TrialsInfo::getType() const
{
    return type_;
}

void Habit::TrialsInfo::setType(int type)
{
    type_ = type;
}
#endif

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
