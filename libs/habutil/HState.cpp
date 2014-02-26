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

HState::HState(const QString& name, QState* parent )
: QState(parent)
, m_name(name)
, m_prefix("")
{};

HState::HState(const QString& name, const QString& prefix, QState* parent)
: QState(parent)
, m_name(name)
, m_prefix(prefix)
{};

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
	
    qDebug() << "Exiting state:" << state;
}

