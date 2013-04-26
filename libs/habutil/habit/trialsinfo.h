
#ifndef HABIT_TRIALS_INFO_H
#define HABIT_TRIALS_INFO_H

#include "HTypes.h"
#include <QDataStream>

/// \file trialsinfo.h
/// \brief Declaraction of class Habit::TrialsInfo

/// Common namespace for all entities of the Habit
namespace Habit
{
   /// Stores trials info.
class TrialsInfo
{
public:
    TrialsInfo(const HTrialCompletionType& ctype = HTrialCompletionType::HTrialCompletionFixedLength);
    ~TrialsInfo();

public:
#if 0
    enum TrialType{
		eFixedLength = 0,
        eSubjectControlled = 1
    };
#endif
    const HTrialCompletionType& getTrialCompletionType() const;
    void setTrialCompletionType(const HTrialCompletionType& ctype);

#if 0
    int getType() const;
    void setType(int type);
#endif
    unsigned int getLength() const;
    void setLength(unsigned int length);
    unsigned int getLookTimes() const;
    void setLookTimes(unsigned int lookTimes);
    unsigned int getNumberOfTrials() const;
    void setNumberOfTrials(unsigned int number);

private:
	const HTrialCompletionType* pctype_;
	unsigned int length_;
	unsigned int lookTimes_;
	unsigned int  number_;
};

QDataStream & operator<< (QDataStream& stream, Habit::TrialsInfo info);
QDataStream & operator>> (QDataStream& stream, Habit::TrialsInfo& info);
bool operator==(const Habit::TrialsInfo& lhs, const Habit::TrialsInfo& rhs);

} //namespace Habit

#endif // HABIT_TRIALS_INFO_H
