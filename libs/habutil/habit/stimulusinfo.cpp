#include "stimulusinfo.h"
#include <QFile>

using namespace Habit;

StimulusInfo::StimulusInfo()
: id_(-1)
, name_(QString(""))
, fileName_(QString(""))
, isLoopPlayBack_(false)
, audioBalance_(50)
, isBackground_(false)
, isColor_(false)
, color_(QColor())
{
}

StimulusInfo::StimulusInfo(const QString& name, const QString& filename, bool is_loop, float audio, bool isBackground, bool isColor, QColor color)
: id_(-1)
, name_(name)
, fileName_(filename)
, isLoopPlayBack_(is_loop)
, audioBalance_(audio)
, isBackground_(isBackground)
, isColor_(isColor)
, color_(color)
{}

StimulusInfo::StimulusInfo(const QString& filename, bool is_loop, float audio, bool isBackground, bool isColor, QColor color)
: id_(-1)
, name_(QString())
, fileName_(filename)
, isLoopPlayBack_(is_loop)
, audioBalance_(audio)
, isBackground_(isBackground)
, isColor_(isColor)
, color_(color)
{}

StimulusInfo::StimulusInfo(const QColor& color)
: id_(-1)
, name_(QString())
, fileName_(QString())
, isLoopPlayBack_(false)
, audioBalance_(0)
, isBackground_(false)
, isColor_(true)
, color_(color)
{}

StimulusInfo::StimulusInfo(const StimulusInfo& info)
: id_(info.getId())
, name_(info.getName())
, fileName_(info.getFileName())
, isLoopPlayBack_(info.isLoopPlayBack())
, audioBalance_(info.getAudioBalance())
, isBackground_(info.isBackground())
, isColor_(info.isColor())
, color_(info.getColor())
{}

StimulusInfo::StimulusInfo(const StimulusInfoOld& oldInfo)
: id_(-1)
, name_(oldInfo.getName())
, fileName_(oldInfo.getFileName())
, isLoopPlayBack_(oldInfo.isLoopPlayBack())
, audioBalance_(oldInfo.getVolume())
, isBackground_(false)
, isColor_(false)
, color_(QColor())
{}

StimulusInfo::~StimulusInfo()
{
}

int StimulusInfo::getId() const
{
	return id_;
}

void StimulusInfo::setId(int id)
{
	id_ = id;
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

QString StimulusInfo::getAbsoluteFileName(const QDir& stimBaseDir) const
{
	QDir f(fileName_);
	if (f.isRelative())
	{
		return stimBaseDir.absoluteFilePath(fileName_);
	}
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

int StimulusInfo::getVolume() const
{
	return audioBalance_;
}

void StimulusInfo::setAudioBalance(int audioBalance)
{
    audioBalance_ = audioBalance;
}

void StimulusInfo::setVolume(int volume)
{
	audioBalance_ = volume;
}

bool StimulusInfo::isBackground() const
{
	return isBackground_;
}

void StimulusInfo::setIsBackground(bool b)
{
	isBackground_ = b;
}

bool StimulusInfo::isColor() const
{
	return isColor_;
}

void StimulusInfo::setIsColor(bool b)
{
	isColor_ = b;
}

QColor StimulusInfo::getColor() const
{
	return color_;
}

void StimulusInfo::setColor(const QColor& color)
{
	color_ = color;
}

bool StimulusInfo::isJPG() const
{
	if (isBackground() || isColor())
		return false;
	QString f = getFileName();
	if (f.size() == 0)
		return false;
	return (f.endsWith("jpg", Qt::CaseInsensitive) || f.endsWith("jpeg", Qt::CaseInsensitive));
}

bool StimulusInfo::isPNG() const
{
	if (isBackground() || isColor())
		return false;
	QString f = getFileName();
	if (f.size() == 0)
		return false;
	return f.endsWith("png", Qt::CaseInsensitive);
}


QDataStream & Habit::operator << (QDataStream& stream, const StimulusInfo& settings)
{
	stream << settings.getName() << settings.getFileName() << settings.isLoopPlayBack() << settings.getAudioBalance() <<
			settings.isBackground() << settings.isColor() << settings.getColor();
	return stream;
}

QDataStream & Habit::operator>> (QDataStream& stream, StimulusInfo& info)
{
	QString name;
	QString fileName;
	bool isLoopPlayBack;
	int audioBalance;
	bool isBackground, isColor;
	QColor color;
	stream >> name >> fileName >> isLoopPlayBack >> audioBalance >> isBackground >> isColor >> color;
	info.setName(name);
	info.setFileName(fileName);
	info.setLoopPlayBack(isLoopPlayBack);
	info.setAudioBalance(audioBalance);
	info.setIsBackground(isBackground);
	info.setIsColor(isColor);
	info.setColor(color);
	return stream;
}

QString StimulusInfo::label() const
{
	if (isBackground()) return QString("Background");
	else return getFileName();
}


#if 0
QTextStream & Habit::operator << (QTextStream& stream, const StimulusInfo& settings)
{
//	stream << settings.getFileName() << " " << settings.getVolume() << " " << settings.isLoopPlayBack() ? "yes" : "no";
	return stream;
}

QTextStream & Habit::operator>> (QTextStream& stream, StimulusInfo& settings)
{
	QString s;

	// initialize
	settings.setFileName("");
	settings.setVolume(0);
	settings.setLoopPlayBack(false);
	settings.setName("");

	// read filename
	stream >> s;
	settings.setFileName(s);

	// next field is volume (if present, and if its an int)


	return stream;
}
#endif

QDebug Habit::operator<<(QDebug dbg, const StimulusInfo& info)
{
	dbg.nospace() << "StimulusInfo: Id " << info.getId() << " File " << (info.getFileName().isEmpty() ? QString("empty") : info.getFileName()) << " Loop? " << info.isLoopPlayBack() << " Vol " << info.getAudioBalance() << " Bkgd? " << info.isBackground() << " color? " << info.isColor() << " color " << info.getColor() << endl;
	return dbg.nospace();
}

bool Habit::operator==(const Habit::StimulusInfo& lhs, const Habit::StimulusInfo& rhs)
{
	return 	lhs.getId() == rhs.getId() &&
			lhs.getName() == rhs.getName() &&
			lhs.getFileName() == rhs.getFileName() &&
			lhs.isLoopPlayBack() == rhs.isLoopPlayBack() &&
			lhs.getAudioBalance() == rhs.getAudioBalance() &&
			lhs.isBackground() == rhs.isBackground() &&
			lhs.isColor() == rhs.isColor() &&
			lhs.getColor() == rhs.getColor();
}





/*
 * Retain this "old" version so we can read older data.
 */

StimulusInfoOld::StimulusInfoOld()
{
	name_ = QString("");
    fileName_ = QString("");
    isLoopPlayBack_ = true;
    audioBalance_ = 50;
}

StimulusInfoOld::StimulusInfoOld(const QString& name, const QString& filename, bool is_loop, float audio, bool isBackground)
	: name_(name)
	, fileName_(filename)
	, isLoopPlayBack_(is_loop)
	, audioBalance_(audio)
{}

StimulusInfoOld::StimulusInfoOld(const StimulusInfoOld& info)
: name_(info.getName())
, fileName_(info.getFileName())
, isLoopPlayBack_(info.isLoopPlayBack())
, audioBalance_(info.getAudioBalance())
{}

StimulusInfoOld::~StimulusInfoOld()
{
}

QString StimulusInfoOld::getName() const {
	return name_;
}

void StimulusInfoOld::setName(const QString& name) {
	name_ = name;
}

QString StimulusInfoOld::getFileName() const
{
    return fileName_;
}

void StimulusInfoOld::setFileName(const QString& fileName)
{
    fileName_ = fileName;
}

bool StimulusInfoOld::isLoopPlayBack() const
{
    return isLoopPlayBack_;
}

void StimulusInfoOld::setLoopPlayBack(bool isLoopPlayBack)
{
    isLoopPlayBack_ = isLoopPlayBack;
}

int StimulusInfoOld::getAudioBalance() const
{
    return audioBalance_;
}

int StimulusInfoOld::getVolume() const
{
	return audioBalance_;
}

void StimulusInfoOld::setAudioBalance(int audioBalance)
{
    audioBalance_ = audioBalance;
}

void StimulusInfoOld::setVolume(int volume)
{
	audioBalance_ = volume;
}

QDataStream & Habit::operator << (QDataStream& stream, const StimulusInfoOld& settings)
{
	stream << settings.getName();
	stream << settings.getFileName();
	stream << settings.isLoopPlayBack();
	stream << settings.getAudioBalance();
	return stream;
}

QDataStream & Habit::operator>> (QDataStream& stream, StimulusInfoOld& settings)
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

#if 0
QTextStream & Habit::operator << (QTextStream& stream, const StimulusInfoOld& settings)
{
//	stream << settings.getFileName() << " " << settings.getVolume() << " " << settings.isLoopPlayBack() ? "yes" : "no";
	return stream;
}

QTextStream & Habit::operator>> (QTextStream& stream, StimulusInfoOld& settings)
{
	QString s;

	// initialize
	settings.setFileName("");
	settings.setVolume(0);
	settings.setLoopPlayBack(false);
	settings.setName("");

	// read filename
	stream >> s;
	settings.setFileName(s);

	// next field is volume (if present, and if its an int)


	return stream;
}


QDebug Habit::operator<<(QDebug dbg, const StimulusInfoOld& info)
{
	dbg.nospace() << "StimulusInfoOld: File " << (info.getFileName().isEmpty() ? QString("empty") : info.getFileName()) << " Loop? " << info.isLoopPlayBack() << " Bal " << info.getAudioBalance() << endl;
	return dbg.nospace();
}

bool Habit::operator==(const Habit::StimulusInfoOld& lhs, const Habit::StimulusInfoOld& rhs)
{
	return lhs.getName() == rhs.getName() && lhs.getFileName() == rhs.getFileName() && lhs.isLoopPlayBack() == rhs.isLoopPlayBack() && lhs.getAudioBalance() == rhs.getAudioBalance();
}

#endif






