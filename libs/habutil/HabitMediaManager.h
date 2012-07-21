/*
 *  HabitMediaManager.h
 *  habutil
 *
 *  Created by Oakes Lab on 7/20/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */


#include <QObject>
#include <QList>
#include "HabitPlayer.h"

class HabitMediaManager : public QObject
{
	Q_OBJECT

private:
	QList<HabitPlayer *> m_players;
	bool m_pendingStartSignal;
	
public:

	HabitMediaManager(): QObject(), m_pendingStartSignal(false) {};
	~HabitMediaManager() {};	// TODO: should this destructor do something? 

	void addPlayer(HabitPlayer* player);
	
public slots:

	void stim(int);
	void ag();
	void playerStarted(int i);

signals:

	void started();
};