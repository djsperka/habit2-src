
#ifndef HABIT_TRIALS_INFO_H
#define HABIT_TRIALS_INFO_H

/// \file trialsinfo.h
/// \brief Declaraction of class Habit::TrialsInfo

/// Common namespace for all entities of the Habit
namespace Habit
{
   /// Stores trials info.
class TrialsInfo
{
public:
    TrialsInfo();
    ~TrialsInfo();

public:
    enum TrialType{
		eFixedLength = 0,
        eSubjectControlled = 1
    };
    int getType() const;
    void setType(int type);
    unsigned int getLength() const;
    void setLength(unsigned int length);
    unsigned int getLookTimes() const;
    void setLookTimes(unsigned int lookTimes);
    unsigned int getNumberOfTrials() const;
    void setNumberOfTrials(unsigned int number);

private:
	int type_;
	unsigned int length_;
	unsigned int lookTimes_;
	unsigned int  number_;
};

} //namespace Habit

#endif // HABIT_TRIALS_INFO_H
