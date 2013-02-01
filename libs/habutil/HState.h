/*
 *  HState.h
 *  hstate
 *
 *  Created by Oakes Lab on 6/7/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _HSTATE_H_
#define _HSTATE_H_

#include <QState>

class HState : public QState
{
    Q_OBJECT
public:
    HState(const QString& name, QState* parent = 0 );
    HState(const QString& name, const QString& prefix, QState* parent = 0 );
	
    QString name() const { return m_name; }
    QString prefix() const { return m_prefix; }
	
	public slots:
    void setName( const QString& name ) { m_name = name; }
    void setPrefix( const QString& prefix ) { m_prefix = prefix; }
	
protected:
    virtual void onEntry( QEvent* e );
    virtual void onExit( QEvent* e );
	
protected:
    QString m_name;
    QString m_prefix;
};

#endif
