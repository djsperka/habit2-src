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
	if (!pinfo->getFileName().isEmpty() && !pinfo->isBackground())
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
		// IMAGE types are not buffered.

		QFile file(filename);
		m_filename = filename;
		if (file.exists()) 
		{
			if (isImageFile(filename))
			{
				//m_pImage = new QImage(filename);
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

void HStimulusSource::loadBuffer()
{
	if (isVideo() || isAudio())
	{
		// Load file into a QByteArray, and create a QBuffer that manages it.
		qDebug() << "HStimulusSource::loadBuffer - Buffering video/audio " << m_filename;
		QFile file(m_filename);
		if (!file.open(QIODevice::ReadOnly))
		{
			qCritical() << "Cannot read stimulus file " << m_filename;
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
		qDebug() << "HStimulusSource::loadBuffer - not buffered, not video/audio " << m_filename;
	}
}

void HStimulusSource::freeBuffer()
{
	if (hasBuffer())
	{
		delete m_pBuffer;
		delete m_pByteArray;
		m_pBuffer = (QBuffer*) NULL;
		m_pByteArray = (QByteArray*) NULL;
	}
	else if (hasImage())
	{
		freeImage();
	}
}


void HStimulusSource::loadImage(const QSize& parentSize, bool bFullScreen, bool bMaintainAspectRatio)
{
	QImage image(m_filename);
	//m_pImage = new QImage(m_filename);
	QSize size = image.size();
	if (bFullScreen)
	{
		double scaleX = size.width()*1.0 / parentSize.width();
		double scaleY = size.height()*1.0 / parentSize.height();
		if (bMaintainAspectRatio)
		{
			if (scaleX < scaleY)
			{
				m_pImage = new QImage(image.scaledToHeight(parentSize.height(), Qt::SmoothTransformation));
			}
			else
			{
				m_pImage = new QImage(image.scaledToWidth(parentSize.width(), Qt::SmoothTransformation));
			}
		}
		else
		{
			m_pImage = new QImage(image.scaled(parentSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
		}
		qDebug() << "HStimulusSource::loadImage: " << m_filename;
		qDebug() << "Original image size " << image.size();
		qDebug() << "Scaled image size " << m_pImage->size();
		//qDebug() << "            FSimage  size " << m_pImage->size().width() << "x" << m_pImage->size().height();
	}
}

void HStimulusSource::freeImage()
{
	if (m_pImage)
	{
		delete m_pImage;
		m_pImage = (QImage *)NULL;
	}
}




HStimulusSource::~HStimulusSource()
{
//	if (m_pVideo) delete m_pVideo;
//	if (m_pBuffer) delete m_pBuffer;
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
