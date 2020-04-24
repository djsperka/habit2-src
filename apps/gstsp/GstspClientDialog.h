/*
 * GstspClientDialog.h
 *
 *  Created on: Apr 15, 2020
 *      Author: dan
 */

#ifndef GSTSPCLIENTDIALOG_H_
#define GSTSPCLIENTDIALOG_H_

#include <QDialog>
#include <QTcpSocket>

// forward dec
namespace hmm {
	class HMM;
}
namespace Ui {
	class gstspclientdialog;
}
class QString;
class QPlainTextEdit;
class QDataStream;


class GstspClientDialog: public QDialog
{
	Q_OBJECT

private:
	Ui::gstspclientdialog *m_pui;
	hmm::HMM *m_pmm;
	QTcpSocket *m_ptcpSocket;
	QDataStream m_dataStream;

private slots:
	void readServerCommand();
	void displayError(QAbstractSocket::SocketError socketError);
	void executeServerCommand(const char *cmd);

public:
	GstspClientDialog(const QString& addr, int port, hmm::HMM *phmm, QWidget *parent=0);
	~GstspClientDialog();
	void components();
};


#endif /* GSTSPCLIENTDIALOG_H_ */
