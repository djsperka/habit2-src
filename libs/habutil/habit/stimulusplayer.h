#ifndef STIMULUSPLAYER_H
#define STIMULUSPLAYER_H

#include <QtGui/QWidget>

class StimulusPlayer : public QWidget
{
    Q_OBJECT

public:
    StimulusPlayer(QWidget *w = 0);
    ~StimulusPlayer();
};

#endif // STIMULUSPLAYER_H
