
#ifndef HABIT_ATTENTION_GETTER_SETTINGS_H
#define HABIT_ATTENTION_GETTER_SETTINGS_H

/// \file attentiongettersettings.h
/// \brief Declaraction of class Habit::AttentionGetterSettings

#include "stimulussettings.h"
#include <QColor>
#include <QDataStream>

namespace Habit {

/// Stores attention getter settings.
class AttentionGetterSettings
{
public:
    AttentionGetterSettings();
    AttentionGetterSettings(const AttentionGetterSettings& settings);
    ~AttentionGetterSettings();
    AttentionGetterSettings& operator=(const AttentionGetterSettings& rhs);
    AttentionGetterSettings clone();

public:
	int getId() const;
	void setId(int);
    bool isAttentionGetterUsed() const;
    void setUseAttentionGetter(bool isAttentionGetterUsed);
    const StimulusSettings& getAttentionGetterStimulus() const;
    StimulusSettings& getAttentionGetterStimulus();
    void setAttentionGetterStimulus(const StimulusSettings& attentionGetterStimulus);
    QColor getBackGroundColor() const;
    void setBackGroundColor(const QColor& backGroundColor);
	void loadFromDB(size_t id);
	bool saveToDB(size_t id_);

private:
	int id_;
	bool isAttentionGetterUsed_;
	StimulusSettings attentionGetterStimulus_;
	QColor backGroundColor_;
};

QDataStream & operator<< (QDataStream& stream, const AttentionGetterSettings& settings);
QDataStream & operator>> (QDataStream& stream, AttentionGetterSettings& settings);
bool operator==(const Habit::AttentionGetterSettings& lhs, const Habit::AttentionGetterSettings& rhs);


} // namespace Habit

#endif // HABIT_ATTENTION_GETTER_SETTINGS_H
