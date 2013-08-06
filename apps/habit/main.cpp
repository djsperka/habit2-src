#include "HApplication.h"
#include "HMainWindow.h"
#include "version.h"
#include <QApplication>
#include <QTime>
#include <QDesktopServices>
#include <QTextStream>
#include <QFile>
#include <QtDebug>
#include <QMenuBar>
#include <QString>
#include <iostream>

using namespace std;
QTextStream logfile; 

#ifndef USING_QT5
void SimpleLoggingHandler(QtMsgType type, const char *msg) 
{    
	switch (type) 
	{
	case QtDebugMsg:
		logfile << QTime::currentTime().toString("hh:mm:ss.zzz") << " Debug: " << msg << "\n";
		break;
	case QtCriticalMsg:
		logfile << QTime::currentTime().toString("hh:mm:ss.zzz") << " Critical: " << msg << "\n";
		break;
	case QtWarningMsg:
		logfile << QTime::currentTime().toString("hh:mm:ss.zzz") << " Warning: " << msg << "\n";
		break;
	case QtFatalMsg:
		logfile << QTime::currentTime().toString("hh:mm:ss.zzz") <<  " Fatal: " << msg << "\n";
		abort();
		break;
	}
}

void ScreenLoggingHandler(QtMsgType type, const char* msg)
{
    const char symbols[] = { 'I', 'E', '!', 'X' };
    QString output = QString("[%1] %2").arg( symbols[type] ).arg( msg );
    std::cerr << output.toStdString() << std::endl;
    if( type == QtFatalMsg ) abort();
}
#else
void SimpleLoggingHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	Q_UNUSED(context);
	switch (type)
	{
	case QtDebugMsg:
		logfile << QTime::currentTime().toString("hh:mm:ss.zzz") << " Debug: " << msg << "\n";
		break;
	case QtCriticalMsg:
		logfile << QTime::currentTime().toString("hh:mm:ss.zzz") << " Critical: " << msg << "\n";
		break;
	case QtWarningMsg:
		logfile << QTime::currentTime().toString("hh:mm:ss.zzz") << " Warning: " << msg << "\n";
		break;
	case QtFatalMsg:
		logfile << QTime::currentTime().toString("hh:mm:ss.zzz") <<  " Fatal: " << msg << "\n";
		abort();
		break;
	}
}

void ScreenLoggingHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	Q_UNUSED(context);
    const char symbols[] = { 'I', 'E', '!', 'X' };
    QString output = QString("[%1] %2").arg( symbols[type] ).arg( msg );
    std::cerr << output.toStdString() << std::endl;
    if( type == QtFatalMsg ) abort();
}


#endif

int main(int argc, char *argv[])
{
	int i;
	bool bs = false;
	for (i=0; i<argc; i++)
	{
		cout << argv[i] << endl;
		if (!strcmp(argv[i], "-s"))
		{
			bs = true;
		}
	}
	if (bs)
	{
#ifndef USING_QT5
		qInstallMsgHandler(ScreenLoggingHandler);
#else
		qInstallMessageHandler(ScreenLoggingHandler);
#endif
	}
	else 
	{
#ifndef USING_QT5
		qInstallMsgHandler(SimpleLoggingHandler);
#else
		qInstallMessageHandler(SimpleLoggingHandler);
#endif
	}

    HApplication h(argc, argv);
	h.setApplicationVersion(HABIT_VERSION);
	h.setApplicationName("Habit");
	h.setOrganizationName("Infant Cognition Lab");
	h.setOrganizationDomain("infantcognitionlab.ucdavis.edu");

	// Generate filename after application object initialized -- this sets data location based on application/organization name.
	QString file_name;// = QDesktopServices::storageLocation(QDesktopServices::DataLocation) ;
#ifndef USING_QT5
	file_name = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#else
	file_name = QStandardPaths::standardLocations(QStandardPaths::DataLocation)[0];
#endif


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
	qDebug() << "Application Start version " << HABIT_VERSION;
	HMainWindow w;
	QObject::connect(&h, SIGNAL(showResultsFile(QString)), &w, SLOT(showResultsFile(QString)));
	Q_INIT_RESOURCE(resources);
    w.show();
    return h.exec();
}

