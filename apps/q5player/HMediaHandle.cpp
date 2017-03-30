/*
 * HMediaHandle.cpp
 *
 *  Created on: Feb 28, 2017
 *      Author: dan
 */

#include "HMediaHandle.h"
#include <QUrl>

HMediaHandle::HMediaHandle(const QString& filename, bool goAheadAndLoad)
: m_content(QUrl::fromLocalFile(filename))
, m_file(filename)
, m_isLoaded(false)
{
	if (goAheadAndLoad)
		load();
}

HMediaHandle::~HMediaHandle() {
	// TODO Auto-generated destructor stub
}


QIODevice *HMediaHandle::iodevice()
{
	load();	// only works once, will throw if fails to load
	m_buffer.setBuffer(&m_ba);
	return &m_buffer;
}

void HMediaHandle::load()
{
	if (!m_isLoaded)
	{
		if (!m_file.exists())
			throw std::invalid_argument(QString("HMediaHolder::load() - file does not exist: %1").arg(m_file.fileName()).toStdString());
		if (!m_file.open(QIODevice::ReadOnly))
			throw std::invalid_argument(QString("HMediaHolder::load() - cannot open file: %1").arg(m_file.fileName()).toStdString());

		// TODO need to check size/error here?
		m_ba = m_file.readAll();
		m_isLoaded = true;
	}
}


