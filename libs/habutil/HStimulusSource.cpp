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
#include <QDir>
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

HStimulusSource::HStimulusSource(const Habit::StimulusInfo* pinfo, const QDir& stimRootDir, bool buffer)
: m_type(BACKGROUND)
, m_pBuffer(0)
, m_pImage(0)
, m_audioBalance(pinfo->getVolume())
, m_isLooped(pinfo->isLoopPlayBack())
{
	if (!pinfo->getFileName().isEmpty())
		init(pinfo->getAbsoluteFileName(stimRootDir), buffer);
	else
		init();
}


HStimulusSource::HStimulusSource()
: m_type(BACKGROUND)
, m_pByteArray(0)
, m_pBuffer(0)
, m_pImage(0)
, m_audioBalance(0)
, m_isLooped(false)
{
	init();
}


void HStimulusSource::init(const QString& filename, bool buffer)
{
	// Zero length filename implies BACKGROUND.
	if (filename.length() > 0)
	{
		// Figure out which stimulus type this is.
		// If there's a filename, then its either IMAGE, AUDIO or VIDEO.
		// Each type has a different method for loading. In addition,
		// VIDEO types may be buffered or not.

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
				m_type = VIDEO;

				// if the file is to be buffered, then read it now.
				// The player will know the file is buffered if the buffer itself
				// is non-null. Unbuffered videos are played from disk.
				if (buffer)
				{
					// Load file into a QByteArray, and create a QBuffer that manages it.
					qDebug() << "Buffering stimulus " << filename;
					if (!file.open(QIODevice::ReadOnly))
					{
						qCritical() << "Cannot read video stimulus file " << filename;
					}
					else
					{
						m_pByteArray = new QByteArray(file.readAll());
						m_pBuffer = new QBuffer(m_pByteArray);
						file.close();
					}
				}
				else
				{
					qDebug() << "Unbuffered stimulus " << filename;
				}
			}
		}
		else 
		{
			qCritical() << "HStimulusSource file not found: \"" << filename << "\"";
			m_type = ERROR;
		}
	}
	else
	{
		m_type = BACKGROUND;
	}
}


HStimulusSource::~HStimulusSource()
{
//	if (m_pVideo) delete m_pVideo;
//	if (m_pBuffer) delete m_pBuffer;

	// TODO Having a lot of trouble getting this cleanup to work without crashing.
	// In some cases Qt takes ownership of objects, and I think I keep running into double deletes.
	// I'm just commenting out the delete(s) here, yes its a memory leak. Deal with it.
	//if (m_pImage) delete m_pImage;
	//if (m_pBuffer) delete m_pBuffer;
	//if (m_pByteArray) delete m_pByteArray;
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
