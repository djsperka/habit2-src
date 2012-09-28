/*
 *  StimulusSource.h
 *  myp
 *
 *  Created by Oakes Lab on 5/25/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef STIMULUSSOURCE_H
#define STIMULUSSOURCE_H

#include <Phonon/MediaSource>
#include <QImage>
#include <QTextStream>
#include <QBuffer>

class HStimulusSource
{
public:
	
	enum HStimulusSourceType {
		BACKGROUND = 0, VIDEO, IMAGE, AUDIO, EMPTY, ERROR
	};
	
	HStimulusSource();
	HStimulusSource(const QString& filename, int audioBalance = 0, bool islooped=false);

	~HStimulusSource();
	bool isVideo() { return (m_type == VIDEO); };
	QBuffer* buffer() { return m_pBuffer; };
	bool isImage() { return (m_type == IMAGE); };
	bool isAudio() { return (m_type == AUDIO); };
	bool isEmpty() { return (m_type == EMPTY); };
	QImage* image() { return m_pImage; };
	bool isBackground() { return (m_type == BACKGROUND); };
	const HStimulusSourceType type() const { return m_type; };
	const QString& filename() const { return m_filename; };
	const bool isLooped() const { return m_isLooped; };
	const int getAudioBalance() const { return m_audioBalance; };
	
private:
	bool isImageFile(const QString& filename);
	bool isAudioFile(const QString& filename);
	HStimulusSourceType m_type;
	QBuffer *m_pBuffer;
	QImage *m_pImage;
	QString m_filename;
	int m_audioBalance;
	bool m_isLooped;
};


QTextStream& operator<<(QTextStream& out, const HStimulusSource& ss);

#endif
