/*
 *  MediaPlayerDummy.h
 *  hstate
 *
 *  Created by Oakes Lab on 6/7/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */


#include <QObject>
#include <QDebug>

class MediaPlayerDummy: public QObject
{
	Q_OBJECT
	
public:
	
	MediaPlayerDummy() {};
	~MediaPlayerDummy() {};

signals:
	
	void agStarted();
	void stimStarted();
	
public slots:
	
	void play_attention_getter()
	{
		qDebug() << "play_attention_getter: emit agStarted()";
		emit agStarted();
	};
	
	void play_stim()
	{
		qDebug() << "play_stim: emit stimStarted()";
		emit stimStarted();
	};	
	
};


	
