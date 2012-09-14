/*
 *  HMediaManager.h
 *  habutil
 *
 *  Created by Oakes Lab on 7/20/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */


#include <QObject>
#include <QList>
#include "HPlayer.h"

class HMediaManager : public QObject
{
	Q_OBJECT

private:
	QList<HPlayer *> m_players;
	bool m_pendingStartSignal;
	bool m_pendingAGStartSignal;
	
public:

	HMediaManager(): QObject(), m_pendingStartSignal(false), m_pendingAGStartSignal(false) {};
	~HMediaManager() {};	// TODO: should this destructor do something? 

	void addPlayer(HPlayer* player);
	
public slots:

	void stim(int);
	void ag();
	void playerStarted(int i);

signals:
	void agStarted();
	void stimStarted();
};