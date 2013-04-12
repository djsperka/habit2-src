#include "stimulusinfo.h"

using namespace Habit;

StimulusInfo::StimulusInfo()
{
	name_ = QString("");
    fileName_ = QString("");
    isLoopPlayBack_ = true;
    audioBalance_ = 50;
}

StimulusInfo::StimulusInfo(const QString& name, const QString& filename, bool is_loop, float audio)
	: name_(name)
	, fileName_(filename)
	, isLoopPlayBack_(is_loop)
	, audioBalance_(audio)
{}

StimulusInfo::StimulusInfo(const StimulusInfo& info)
: name_(info.getName())
, fileName_(info.getFileName())
, isLoopPlayBack_(info.isLoopPlayBack())
, audioBalance_(info.getAudioBalance())
{}

StimulusInfo::~StimulusInfo()
{
}

QString StimulusInfo::getName() const {
	return name_;
}

void StimulusInfo::setName(const QString& name) {
	name_ = name;
}

QString StimulusInfo::getFileName() const
{
    return fileName_;
}

void StimulusInfo::setFileName(const QString& fileName)
{
    fileName_ = fileName;
}

bool StimulusInfo::isLoopPlayBack() const
{
    return isLoopPlayBack_;
}

void StimulusInfo::setLoopPlayBack(bool isLoopPlayBack)
{
    isLoopPlayBack_ = isLoopPlayBack;
}

int StimulusInfo::getAudioBalance() const
{
    return audioBalance_;
}

void StimulusInfo::setAudioBalance(int audioBalance)
{
    audioBalance_ = audioBalance;
}

QDataStream & Habit::operator << (QDataStream& stream, const StimulusInfo& settings)
{
	stream << settings.getName();
	stream << settings.getFileName();
	stream << settings.isLoopPlayBack();
	stream << settings.getAudioBalance();
	return stream;
}

QDataStream & Habit::operator>> (QDataStream& stream, StimulusInfo& settings)
{
	QString name;
	QString fileName;
	bool isLoopPlayBack;
	int audioBalance;
	stream >> name;
	stream >> fileName;
	stream >> isLoopPlayBack;
	stream >> audioBalance;
	settings.setName(name);
	settings.setFileName(fileName);
	settings.setLoopPlayBack(isLoopPlayBack);
	settings.setAudioBalance(audioBalance);
	return stream;
}

QDebug Habit::operator<<(QDebug dbg, const StimulusInfo& info)
{
	dbg.nospace() << "StimulusInfo: Name " << info.getName() << " Loop? " << info.isLoopPlayBack() << " Bal " << info.getAudioBalance() << endl;
	dbg.nospace() << "              File " << info.getFileName() << endl;
	return dbg.nospace();
}

bool Habit::operator==(const Habit::StimulusInfo& lhs, const Habit::StimulusInfo& rhs)
{
	return lhs.getName() == rhs.getName() && lhs.getFileName() == rhs.getFileName() && lhs.isLoopPlayBack() == rhs.isLoopPlayBack() && lhs.getAudioBalance() == rhs.getAudioBalance();
}


