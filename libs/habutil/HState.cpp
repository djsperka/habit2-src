/*
 *  HState.cpp
 *  hstate
 *
 *  Created by Oakes Lab on 6/7/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HState.h"

#include <QDebug>
#include <QAbstractTransition>

HState::HState( const QString& name, QState* parent )
: QState( parent ), m_name( name )
{
}

void HState::onEntry( QEvent* e )
{
    Q_UNUSED( e );
	
    // Print out the state we are entering and it's parents
    QString state = m_name;
    HState* parent = dynamic_cast<HState*>( parentState() );
    while ( parent != 0 )
    {
        state = parent->name() + "->" + state;
        parent = dynamic_cast<HState*>( parent->parentState() );
    }
    qDebug() << "Entering state:" << state;
}

void HState::onExit( QEvent* e )
{
    Q_UNUSED( e );
	
    // Print out the state we are exiting and it's parents
    QString state = m_name;
    HState* parent = dynamic_cast<HState*>( parentState() );
    while ( parent != 0 )
    {
        state = parent->name() + "->" + state;
        parent = dynamic_cast<HState*>( parent->parentState() );
    }
	
	// Determine the state we're heading...
	QString sTarget;
	QList<QAbstractTransition *> trans = transitions();
	QList<QAbstractTransition *>::const_iterator it;
	for (int i = 0; i<trans.size(); i++)
	{
		HState* t = dynamic_cast<HState*> (trans[i]->targetState());
		if (t) sTarget.append(t->name());
		else sTarget.append("???");
	}
	
    qDebug() << "Exiting state:" << state << " target: " << sTarget;
}

