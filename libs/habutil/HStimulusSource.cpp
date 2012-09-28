/*
 *  HStimulusSource.cpp
 *  myp
 *
 *  Created by Oakes Lab on 5/25/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HStimulusSource.h"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QBuffer>

QTextStream& operator<<(QTextStream& out, const HStimulusSource& ss)
{
	switch (ss.type()) {
		case HStimulusSource::BACKGROUND:
			out << "BACKGROUND";
			break;
		case HStimulusSource::VIDEO:
			out << "VIDEO: " << ss.filename();
			break;
		case HStimulusSource::IMAGE:
			out << "IMAGE: " << ss.filename();
			break;
		case HStimulusSource::AUDIO:
			out << "AUDIO: " << ss.filename();
			break;
		case HStimulusSource::EMPTY:
			out << "EMPTY: ";
			break;
		case HStimulusSource::ERROR:
			out << "ERROR: " << ss.filename();
			break;
		default:
			break;
	}
	return out;
}

HStimulusSource::HStimulusSource() : m_type(BACKGROUND), m_pBuffer(0), m_pImage(0), m_audioBalance(0), m_isLooped(false)
{
}


HStimulusSource::HStimulusSource(const QString& filename, int audioBalance, bool isLooped) : m_type(BACKGROUND), m_pBuffer(0), m_pImage(0), m_audioBalance(audioBalance), m_isLooped(isLooped)
{
	// Zero length filename implies BACKGROUND. 
	if (filename.length() > 0)
	{
		QFile file(filename);
		m_filename = filename;
		if (file.exists()) 
		{
			if (isImageFile(filename))
			{
				m_pImage = new QImage(filename);
				m_type = IMAGE;
			}
			else if (isAudioFile(filename))
			{
				m_type = AUDIO;
			}
			else 
			{
				//file.open(QIODevice::ReadOnly);
				//m_pVideo = new Phonon::MediaSource(file.readAll());
				//QByteArray ba = file.readAll();
				//m_pBuffer = new QBuffer(&ba);
				m_type = VIDEO;
				//file.close();
			}
		}
		else 
		{
			m_type = ERROR;
		}
	}
}


HStimulusSource::~HStimulusSource()
{
//	if (m_pVideo) delete m_pVideo;
//	if (m_pBuffer) delete m_pBuffer;
//	if (m_pImage) delete m_pImage;
}

bool HStimulusSource::isImageFile(const QString& filename)
{
	QFileInfo fileInfo(filename);
	QString suffix = fileInfo.suffix().toUpper();
	return (suffix == "JPG") ||
	(suffix == "JPEG") ||
	(suffix == "PNG") ||
	(suffix == "GIF") ||
	(suffix == "TIF") ||
	(suffix == "TIFF") ||
	(suffix == "PPM") ||
	(suffix == "PGM") ||
	(suffix == "PBM") ||
	(suffix == "PNM") ||
	(suffix == "BMP") ||
	(suffix == "TGA") ||
	(suffix == "PICT") ;
}

bool HStimulusSource::isAudioFile(const QString& filename)
{
	QFileInfo fileInfo(filename);
	QString suffix = fileInfo.suffix().toUpper();
	return	(suffix == "OGG") ||
			(suffix == "MP3") ||
			(suffix == "WMA") ||
			(suffix == "WAV") ||
			(suffix == "AIFF");
}