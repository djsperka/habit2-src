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

class StimulusSource
{
public:
	
	enum StimulusSourceType {
		BACKGROUND = 0, VIDEO, IMAGE, AUDIO, EMPTY, ERROR
	};
	
	StimulusSource();
	StimulusSource(const QString& filename, bool islooped=false);

	~StimulusSource();
	bool isVideo() { return (m_type == VIDEO); };
//	Phonon::MediaSource& video() { return *m_pVideo; };
	QBuffer* buffer() { return m_pBuffer; };
	bool isImage() { return (m_type == IMAGE); };
	bool isAudio() { return (m_type == AUDIO); };
	bool isEmpty() { return (m_type == EMPTY); };
	QImage* image() { return m_pImage; };
	bool isBackground() { return (m_type == BACKGROUND); };
	const StimulusSourceType type() const { return m_type; };
	const QString& filename() const { return m_filename; };
	const bool isLooped() const { return m_isLooped; };
	
private:
	bool isImageFile(const QString& filename);
	bool isAudioFile(const QString& filename);
	StimulusSourceType m_type;
	QBuffer *m_pBuffer;
	QImage *m_pImage;
	QString m_filename;
	bool m_isLooped;
};


QTextStream& operator<<(QTextStream& out, const StimulusSource& ss);

#endif
