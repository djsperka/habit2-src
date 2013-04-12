#include "habitapp.h"
#include <QtGui/QApplication>
#include <QtCore/QTime>
#include <QtGui/QDesktopServices>
#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QtCore/QTDebug>
#include <iostream>

//#include <fstream>
//using namespace std;
QTextStream logfile; 
void SimpleLoggingHandler(QtMsgType type, const char *msg) 
{    
	switch (type) 
	{
	case QtDebugMsg:
		logfile << QTime::currentTime().toString().toAscii().data() << " Debug: " << msg << "\n";
		break;
	case QtCriticalMsg:
		logfile << QTime::currentTime().toString().toAscii().data() << " Critical: " << msg << "\n";
		break;
	case QtWarningMsg:
		logfile << QTime::currentTime().toString().toAscii().data() << " Warning: " << msg << "\n";
		break;
	case QtFatalMsg:
		logfile << QTime::currentTime().toString().toAscii().data() <<  " Fatal: " << msg << "\n";
		abort();
		break;
	}
}

int main(int argc, char *argv[])
{
	qInstallMsgHandler(SimpleLoggingHandler);
    QApplication a(argc, argv);
	a.setApplicationVersion("1.10.01");
	a.setApplicationName("Habit");
	a.setOrganizationName("Infant Cognition Lab");
	a.setOrganizationDomain("infantcognitionlab.ucdavis.edu");

	// Generate filename after application object initialized -- this sets data location based on application/organization name.
	QString file_name = QDesktopServices::storageLocation(QDesktopServices::DataLocation) ;
#ifdef Q_OS_MAC
	file_name += QString("/habit-%1.log").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm"));
#else
	file_name += QString("\\habit-%1.log").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm"));
#endif
	QFile file(file_name);
	QTextStream stream(stdout);
	stream << file_name;
	file.open(QIODevice::Append | QIODevice::Text);
	logfile.setDevice(&file);
	logfile.flush();
	qDebug() << "Application Start version " << "1.10.01";
	HabitApp w;
    w.show();
    return a.exec();
}
