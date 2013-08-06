#ifndef STIMULUSPLAYER_H
#define STIMULUSPLAYER_H

#ifdef USING_QT5
#include <QtWidgets/QWidget>
#else
#include <QtGui/QWidget>
#endif

class StimulusPlayer : public QWidget
{
    Q_OBJECT

public:
    StimulusPlayer(QWidget *w = 0);
    ~StimulusPlayer();
};

#endif // STIMULUSPLAYER_H
