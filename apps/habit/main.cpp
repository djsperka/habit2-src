#include "HApplication.h"
#include "H2MainWindow.h"
#include "HWorkspaceUtil.h"
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
QFile *f_pFileLog = NULL;
QTextStream f_streamFileLog;
bool f_bScreenLog = false;
bool f_bFileLog = false;



void fileLoggingHandler(QtMsgType type, const char *msg)
{    
	switch (type) 
	{
	case QtDebugMsg:
		f_streamFileLog << QTime::currentTime().toString().toAscii().data() << " Debug: " << msg << "\n";
		break;
	case QtCriticalMsg:
		f_streamFileLog << QTime::currentTime().toString().toAscii().data() << " Critical: " << msg << "\n";
		break;
	case QtWarningMsg:
		f_streamFileLog << QTime::currentTime().toString().toAscii().data() << " Warning: " << msg << "\n";
		break;
	case QtFatalMsg:
		f_streamFileLog << QTime::currentTime().toString().toAscii().data() <<  " Fatal: " << msg << "\n";
		abort();
		break;
	}
}

void screenLoggingHandler(QtMsgType type, const char* msg)
{
    const char symbols[] = { 'I', 'E', '!', 'X' };
    QString output = QString("[%1] %2").arg( symbols[type] ).arg( msg );
    cerr << output.toStdString();
    cerr << endl;
    if( type == QtFatalMsg ) abort();
}

void habitLoggingHandler(QtMsgType type, const char *msg)
{
	if (f_bScreenLog)
		screenLoggingHandler(type, msg);
	if (f_bFileLog)
		fileLoggingHandler(type, msg);
	if (type == QtFatalMsg)
		abort();
	return;
}

int main(int argc, char *argv[])
{
	int i;
	bool bPendingWorkspace = false;
	QString sPendingWorkspace;
	bool bDBUpdateOnly = false;

	// by default add debugging output goes to the screen unless we install
	// a handler for logging msgs. Until Habit is at a good release state
	// I'll keep file logging on by default.
	// See below where the arg "-f" is processed. By setting the initialization
	// to false, the following two things will happen:
	// 1. No file logging, screen logging only.
	// 2. We will not install a logging msg handler at all (Qt's default is screen)

	bool bFileLogPending = true;

	// Default is to always have log file.
	for (i=0; i<argc; i++)
	{
		if (!strcmp(argv[i], "-s"))
		{
			f_bScreenLog = true;
		}
		else if (!strcmp(argv[i], "-f"))
		{
			bFileLogPending = true;
		}
		else if (!strcmp(argv[i], "-x"))
		{
			habutilClearWorkspace();
		}
		else if (!strcmp(argv[i], "-w"))
		{
			// TODO - maybe tell user they need to supply a directory?
			if (i < (argc-1))
			{
				bPendingWorkspace = true;
				sPendingWorkspace = argv[i+1];
				i++;
			}
		}
		else if (!strcmp(argv[i], "-D"))
		{
			bDBUpdateOnly = true;
		}
	}

    HApplication h(argc, argv);
	h.setApplicationVersion(HABIT_VERSION);
	h.setApplicationName("habit2");
	h.setOrganizationName("Infant Cognition Lab");
	h.setOrganizationDomain("infantcognitionlab.ucdavis.edu");

	// Open a workspace. The workspace is a folder containing "habit.sqlite" and three folders:
	// "results", "log" and "stim". This MUST be called AFTER the organization name is set in the QApplication
	// object, otherwise the settings file is not found.

	if (bPendingWorkspace)
		habutilSetWorkspace(sPendingWorkspace);

	if (!habutilInitWorkspace() || bDBUpdateOnly)
		return 0;

	GUILib::H2MainWindow w;
	QObject::connect(&h, SIGNAL(showResultsFile(QString)), &w, SLOT(showResultsFile(QString)));
	Q_INIT_RESOURCE(resources);

	// Generate filename after application object initialized -- this sets data location based on application/organization name.
	if (bFileLogPending)
	{
		QString file_name = habutilGetLogDir().absolutePath();
#ifdef Q_OS_MAC
		file_name += QString("/habit-%1.log").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm"));
#else
		file_name += QString("\\habit-%1.log").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm"));
#endif
		f_pFileLog = new QFile(file_name);
		f_pFileLog->open(QIODevice::Append | QIODevice::Text);
		f_streamFileLog.setDevice(f_pFileLog);
		f_streamFileLog.flush();
		f_bFileLog = true;
		qInstallMsgHandler(habitLoggingHandler);
	}
	qDebug() << "Starting Habit, version " << HABIT_VERSION;

	// Now show dialog and start event loop.
    w.show();
    int ii = h.exec();
    if (f_bFileLog)
    {
    	f_streamFileLog.flush();
    	f_pFileLog->close();
    }
    return ii;
}

