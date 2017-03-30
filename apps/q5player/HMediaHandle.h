/*
 * HMediaHandle.h
 *
 *  Created on: Feb 28, 2017
 *      Author: dan
 */

#ifndef APPS_Q5PLAYER_HMEDIAHANDLE_H_
#define APPS_Q5PLAYER_HMEDIAHANDLE_H_


#include <QMediaContent>
#include <QFile>
#include <QBuffer>
#include <QByteArray>
#include <QString>

class HMediaHandle
{
	QMediaContent m_content;
	QFile m_file;
	bool m_isLoaded;
	QBuffer m_buffer;
	QByteArray m_ba;

public:
	HMediaHandle(const QString& filename, bool goAheadAndLoad = true);
	virtual ~HMediaHandle();
	void load();
	const QFile& file() const { return m_file; }
	const QMediaContent& content() const { return m_content; }
	QIODevice* iodevice();
};


#endif /* APPS_Q5PLAYER_HMEDIAHANDLE_H_ */
