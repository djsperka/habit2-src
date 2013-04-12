
#ifndef HABIT_STIMULUS_INFO_H
#define HABIT_STIMULUS_INFO_H

/// \file stimulusinfo.h
/// \brief Declaraction of class Habit::StimulusInfo

#include <QDebug>

/// Common namespace for all entities of the Habit
namespace Habit
{
/// Stores stimulus information.
class StimulusInfo
{
public:
    StimulusInfo();
	StimulusInfo(const QString& name, const QString& filename, bool is_loop, float audio);
    StimulusInfo(const StimulusInfo& );
    ~StimulusInfo();
    //StimulusInfo& operator = (const StimulusInfo&);

public:
    QString getName() const;
	void setName(const QString& name);
    QString getFileName() const;
    void setFileName(const QString& fileName);
    bool isLoopPlayBack() const;
    void setLoopPlayBack(bool isLoopPlayBack);
    int getAudioBalance() const;
    void setAudioBalance(int audioBalance);

	friend bool operator==(const Habit::StimulusInfo& lhs, const Habit::StimulusInfo& rhs);
private:
	QString name_;
	QString fileName_;
	bool isLoopPlayBack_;
	int audioBalance_;
};

QDataStream & operator<< (QDataStream& stream, const StimulusInfo& settings);
QDataStream & operator>> (QDataStream& stream, StimulusInfo& settings);
bool operator==(const Habit::StimulusInfo& lhs, const Habit::StimulusInfo& rhs);
	
QDebug operator<<(QDebug dbg, const StimulusInfo& info);


} //namespace Habit

#endif // HABIT_STIMULUS_INFO_H
