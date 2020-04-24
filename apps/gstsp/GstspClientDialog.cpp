/*
 * GstspClientDialog.cpp
 *
 *  Created on: Apr 15, 2020
 *      Author: dan
 */


#include "GstspClientDialog.h"
#include "ui_gstspclientdialog.h"
#include "HMM.h"
#include <QTcpSocket>
#include <QString>
#include <QStringList>
#include <QDialogButtonBox>
#include <QMessageBox>

GstspClientDialog::GstspClientDialog(const QString& addr, int port, hmm::HMM *phmm, QWidget *parent)
: QDialog(parent)
, m_pui(new Ui::gstspclientdialog)
, m_pmm(phmm)
, m_ptcpSocket(new QTcpSocket(this))
{
    m_pui->setupUi(this);

    // connect OK button
    connect(m_pui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // assign socket to data stream
    m_dataStream.setDevice(m_ptcpSocket);

    // connect signals from the socket
    connect(m_ptcpSocket, &QIODevice::readyRead, this, &GstspClientDialog::readServerCommand);
    connect(m_ptcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
    		this, &GstspClientDialog::displayError);

    // and connect...
    qDebug() << "Client connect to " << addr << ":" << port;
    m_ptcpSocket->connectToHost(addr, port);
}

GstspClientDialog::~GstspClientDialog()
{
}

void GstspClientDialog::displayError(QAbstractSocket::SocketError socketError)
{
	switch (socketError)
	{
	case QAbstractSocket::RemoteHostClosedError:
		break;
	case QAbstractSocket::HostNotFoundError:
		QMessageBox::information(this, "Gstsp Client", QString("The host was not found."));
		break;
	case QAbstractSocket::ConnectionRefusedError:
		QMessageBox::information(this,  "Gstsp Client", "The connection was refused by the peer. "
								  "Make sure the fortune server is running, "
								  "and check that the host name and port "
								  "settings are correct.");
		break;
	default:
		QMessageBox::information(this, "Gstsp Client",
							   QString("The following error occurred: %1.")
							   .arg(m_ptcpSocket->errorString()));
	}
}


void GstspClientDialog::readServerCommand()
{
	char buf[1024];
	m_dataStream.startTransaction();

	qint64 available = m_ptcpSocket->bytesAvailable();

	qint64 nread = m_ptcpSocket->readLine(buf, 1024);
	if (nread > 0)
	{
		m_dataStream.commitTransaction();
		m_pui->plainTextEdit->appendPlainText(buf);
		executeServerCommand(buf);
	}
	else
		m_dataStream.rollbackTransaction();

}

void GstspClientDialog::executeServerCommand(const char *cmd)
{
	// simple parse now - look for "play" and a single word following
	QString scmd(cmd);
	QStringList slcmd = scmd.split(' ');
	qDebug() << "GstspClientDialog::executeServerCommand: " << slcmd;
	// first token is command
	if (slcmd[0] == "play")
	{
		if (slcmd.size()==2)
		{
			m_pmm->play(slcmd[1].toStdString());
		}
	}
}
