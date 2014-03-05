#include "HApplication.h"
#include "HMainWindow.h"
#include "HDBUtil.h"
#include "version.h"
#include <QApplication>
#include <QTime>
#include <QDesktopServices>
#include <QTextStream>
#include <QFile>
#include <QtDebug>
#include <QMenuBar>
#include <iostream>

using namespace std;
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

void ScreenLoggingHandler(QtMsgType type, const char* msg)
{
    const char symbols[] = { 'I', 'E', '!', 'X' };
    QString output = QString("[%1] %2").arg( symbols[type] ).arg( msg );
    std::cerr << output.toStdString() << std::endl;
    if( type == QtFatalMsg ) abort();
}

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
		qInstallMsgHandler(ScreenLoggingHandler);
	}
	else 
	{
		qInstallMsgHandler(SimpleLoggingHandler);
	}

    HApplication h(argc, argv);
	h.setApplicationVersion(HABIT_VERSION);
	h.setApplicationName("Habit");
	h.setOrganizationName("Infant Cognition Lab");
	h.setOrganizationDomain("infantcognitionlab.ucdavis.edu");

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
	qDebug() << "Application Start version " << HABIT_VERSION;
	HMainWindow w;
	QObject::connect(&h, SIGNAL(showResultsFile(QString)), &w, SLOT(showResultsFile(QString)));
	Q_INIT_RESOURCE(resources);

	// test - check for existence of habit.db3 in resources
    QString resourcePath = ":/resources/habit.db3";
    if( true == QFile::exists(resourcePath))
    {
       qDebug() << "habit.db3 found in resource path";
    }
    else
    {
        qDebug() << "habit.db3 NOT found in resource path";
    }

	// Open database here. openDB() looks in the settings for a database. If that's not found then
	// it tries the default location. If a database is found and opened, the function returns true.
	// If no luck, then openDB() returns false and selectDB() is called.
	while (!openDB())
	{
		if (!selectDB(true)) return 0;
	}

	// Now show dialog and start event loop.
    w.show();
    return h.exec();
}

