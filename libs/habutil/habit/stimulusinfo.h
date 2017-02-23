
#ifndef HABIT_STIMULUS_INFO_H
#define HABIT_STIMULUS_INFO_H

/// \file stimulusinfo.h
/// \brief Declaraction of class Habit::StimulusInfo

#include <QDebug>
#include <QDir>
#include <QColor>
#include <QDataStream>

/// Common namespace for all entities of the Habit
namespace Habit
{

	class StimulusInfoOld;

	/// Stores stimulus information.
	class StimulusInfo
	{
	public:
		StimulusInfo();
		StimulusInfo(const QString& name, const QString& filename, bool is_loop=false, float audio=0, bool isBackground = false, bool isColor = false, QColor color = QColor());
		StimulusInfo(const QString& filename, bool is_loop=false, float audio=0, bool isBackground = false, bool isColor = false, QColor color = QColor());
		StimulusInfo(const StimulusInfo& );
		StimulusInfo(const StimulusInfoOld& oldInfo);
		~StimulusInfo();

	public:
		int getId() const;
		void setId(int id);
		QString getName() const;
		void setName(const QString& name);
		QString getFileName() const;
		QString getAbsoluteFileName(const QDir& stimBaseDir) const;
		void setFileName(const QString& fileName);
		bool isLoopPlayBack() const;
		void setLoopPlayBack(bool isLoopPlayBack);
		int getAudioBalance() const;
		void setAudioBalance(int audioBalance);
		int getVolume() const;	// synonym for getAudioBalance
		void setVolume(int volume);	// synonym for setAudioBalance
		bool isBackground() const;
		void setIsBackground(bool b);
		bool isColor() const;
		void setIsColor(bool b);
		QColor getColor() const;
		void setColor(const QColor& color);
		QString label() const;

		friend bool operator==(const Habit::StimulusInfo& lhs, const Habit::StimulusInfo& rhs);
	private:
		int id_;
		QString name_;
		QString fileName_;
		bool isLoopPlayBack_;
		int audioBalance_;
		bool isBackground_;
		bool isColor_;
		QColor color_;
	};

	class StimulusInfoOld
	{
	public:
		StimulusInfoOld();
		StimulusInfoOld(const QString& name, const QString& filename, bool is_loop=false, float audio=0, bool isBackground = false);
		StimulusInfoOld(const StimulusInfoOld& );
		~StimulusInfoOld();

	public:
		QString getName() const;
		void setName(const QString& name);
		QString getFileName() const;
		void setFileName(const QString& fileName);
		bool isLoopPlayBack() const;
		void setLoopPlayBack(bool isLoopPlayBack);
		int getAudioBalance() const;
		void setAudioBalance(int audioBalance);
		int getVolume() const;	// synonym for getAudioBalance
		void setVolume(int volume);	// synonym for setAudioBalance

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


	QDataStream & operator<< (QDataStream& stream, const StimulusInfoOld& settings);
	QDataStream & operator>> (QDataStream& stream, StimulusInfoOld& settings);

} //namespace Habit

#endif // HABIT_STIMULUS_INFO_H
