#include "HApplication.h"
#include "H2MainWindow.h"
#include "HWorkspaceUtil.h"
#include "version.h"
#include <QtGlobal>  // for QT_VERSION
#if QT_VERSION >= 0x050000
#include <QtMessageHandler>
#endif
#include <QApplication>
#include <QTime>
#include <QDesktopServices>
#include <QTextStream>
#include <QFile>
#include <QtDebug>
#include <QMenuBar>
#include <QProcessEnvironment>
#include <QDesktopWidget>
#include <QMutexLocker>
#include <QMutex>
#include <QCommandLineParser>
#include <iostream>
#include <gst/gst.h>


using namespace std;
QFile *f_pFileLog = NULL;
QTextStream f_streamFileLog;
bool f_bScreenLog = false;
bool f_bFileLog = false;
QMutex f_mutex;


#if QT_VERSION < 0x050000
void fileLoggingHandler(QtMsgType type, const char *msg)
{    
	switch (type) 
	{
	case QtDebugMsg:
		f_streamFileLog << QTime::currentTime().toString() << " Debug: " << msg << "\n";
		break;
	case QtCriticalMsg:
		f_streamFileLog << QTime::currentTime().toString() << " Critical: " << msg << "\n";
		break;
	case QtWarningMsg:
		f_streamFileLog << QTime::currentTime().toString() << " Warning: " << msg << "\n";
		break;
	case QtFatalMsg:
		f_streamFileLog << QTime::currentTime().toString() <<  " Fatal: " << msg << "\n";
		abort();
		break;
	}
}

void screenLoggingHandler(QtMsgType type, const char *msg)
{
    const char symbols[] = { 'I', 'E', '!', 'X' };
    QString output = QString("[%1] %2").arg( symbols[type] ).arg( msg );
    cerr << output.toStdString();
    cerr << endl;
    if( type == QtFatalMsg ) abort();
}
#else
void fileLoggingHandler(QtMsgType type, const QString& msg)
{
	switch (type)
	{
	case QtDebugMsg:
		f_streamFileLog << QTime::currentTime().toString() << " Debug: " << msg << "\n";
		break;
	case QtInfoMsg:
		f_streamFileLog << QTime::currentTime().toString() << " Info: " << msg << "\n";
		break;
	case QtCriticalMsg:
		f_streamFileLog << QTime::currentTime().toString() << " Critical: " << msg << "\n";
		break;
	case QtWarningMsg:
		f_streamFileLog << QTime::currentTime().toString() << " Warning: " << msg << "\n";
		break;
	case QtFatalMsg:
		f_streamFileLog << QTime::currentTime().toString() <<  " Fatal: " << msg << "\n";
		abort();
		break;
	}
}

void screenLoggingHandler(QtMsgType type, const QString& msg)
{
    char symbol;
	switch (type)
	{
	case QtDebugMsg:
		symbol = 'D';
		break;
	case QtInfoMsg:
		symbol = 'I';
		break;
	case QtCriticalMsg:
		symbol = 'C';
		break;
	case QtWarningMsg:
		symbol = 'W';
		break;
	case QtFatalMsg:
		symbol = 'F';
		break;
	}

    QString output = QString("[%1] %2").arg( symbol ).arg( msg );
    cerr << output.toStdString();
    cerr << endl;
    if( type == QtFatalMsg ) abort();
}

#endif
#if QT_VERSION < 0x050000
void habitLoggingHandler(QtMsgType type, const char *msg)
#else
void habitLoggingHandler(QtMsgType type, const QMessageLogContext &, const QString& msg)
#endif
{
	QMutexLocker locker(&f_mutex);
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
	bool bTestRunIsDefault = false;
	bool bShowTestingIcon = false;
	bool bEditTemplates = false;
	bool bStimInDialog = false;
	bool bNotInstalled = false;		// use -n to set. If unset, use system plugin path relative to exe in ../GStreamerPlugins

	// by default add debugging output goes to the screen unless we install
	// a handler for logging msgs. Until Habit is at a good release state
	// I'll keep file logging on by default.
	// See below where the arg "-f" is processed. By setting the initialization
	// to false, the following two things will happen:
	// 1. No file logging, screen logging only.
	// 2. We will not install a logging msg handler at all (Qt's default is screen)


	bool bFileLogPending = true;

	// create application
    HApplication h(argc, &argv);
	h.setApplicationVersion(HABIT_VERSION);
	h.setApplicationName("habit2");
	h.setOrganizationName("Infant Cognition Lab");
	h.setOrganizationDomain("infantcognitionlab.ucdavis.edu");

	QCommandLineParser parser;
    parser.setApplicationDescription("Habit - Infant cognition stimulus presentation and DAQ application");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOptions({
             {"s", "write log to screen"},
			 {"f", "write log to file" },
			 {"x", "clear workspace value in preferences."},
			 {"w", "set workspace folder", "workspace"},
			 {"D", "check (and update if necessary) database version for current workspace"},
			 {"T", "\"Test run\" is default when running an experiment"},
			 {"t", "Show testing icon on toolbar"},
			 {"e", "edit templates"},
			 {"z", "Show stimuli in a dialog - for developing experiments."},
			 {"n", "Run habit as if it is NOT installed - developers only!"},
         });
    parser.process(h);

	f_bScreenLog = parser.isSet("s");
	bFileLogPending = parser.isSet("f");
    	if (parser.isSet("x"))
    	{
    		habutilClearWorkspace();
    	}
    if (parser.isSet("w"))
    	{
    		bPendingWorkspace = true;
    		sPendingWorkspace = parser.value("w");
    	}
    bDBUpdateOnly = parser.isSet("D");
    bTestRunIsDefault = parser.isSet("T");
    bShowTestingIcon = parser.isSet("t");
    bEditTemplates = parser.isSet("e");
	bStimInDialog = parser.isSet("z");
	bNotInstalled = parser.isSet("n");



	// Debug  builds (HABIT_DEBUG) use system-defined gstreamer libs.
	// Release builds (HABIT_RELEASE) _can_ use the system-installed (whatever we link against) gstreamer libs,
	// but the -n flag tells us to run as NOT installed.
	//
	// When running "installed", we assume that the gstreamer framework is embedded in the bundle, and we set two
	// env variables to point that direction:
	// GST_PLUGIN_SYSTEM_PATH - where gstreamer finds plugins. Note that I stick the gst-qt plugin there.
	// GIO_EXTRA_MODULES - some glib thing
	// GST_PLUGIN_SCANNER - scanner checks local plugin cache??

	if (bNotInstalled)
	{
		qDebug() << "Run as NOT installed, exe file is " << argv[0];
	}
	else
	{
		qDebug() << "Run installed, exe file is " << argv[0];

#ifdef HABIT_RELEASE
		QFileInfo fiExe(argv[0]);
		qDebug() << "exePath " << fiExe.path();
		QDir dirScanner(fiExe.path());
		if (dirScanner.cd("../Frameworks/GStreamer.framework/Versions/1.0/libexec/gstreamer-1.0"))
		{
			qputenv("GST_PLUGIN_SCANNER", dirScanner.filePath("gst-plugin-scanner").toLocal8Bit());
		}
		else
		{
			qFatal("Cannot navigate to ../Frameworks/GStreamer.framework/Versions/1.0/libexec/gstreamer-1.0");
		}
		QDir dirPlugins(fiExe.path());
		if (dirPlugins.cd("../Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0"))
		{
			qputenv("GST_PLUGIN_SYSTEM_PATH", dirPlugins.path().toLocal8Bit());
		}
		else
		{
			qFatal("Cannot navigate to ../Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0");
		}
		QDir dirGio(fiExe.path());
		if (dirGio.cd("../Frameworks/GStreamer.framework/Versions/1.0/lib/gio/modules"))
		{
			qputenv("GIO_EXTRA_MODULES", dirGio.path().toLocal8Bit());
		}
		else
		{
			qFatal("Cannot navigate to ../Frameworks/GStreamer.framework/Versions/1.0/lib/gio/modules");
		}
#else
		qDebug() << "DEBUG build will not run installed. TODO.";
#endif
	}

	// Initialize gstreamer
	gst_init (&argc, &argv);

	// Open a workspace. The workspace is a folder containing "habit.sqlite" and three folders:
	// "results", "log" and "stim". This MUST be called AFTER the organization name is set in the QApplication
	// object, otherwise the settings file is not found.

	if (bPendingWorkspace)
		habutilSetWorkspace(sPendingWorkspace);

	if (!habutilInitWorkspace() || bDBUpdateOnly)
		return 0;

	GUILib::H2MainWindow w(bTestRunIsDefault, bShowTestingIcon, bEditTemplates, bStimInDialog);
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
#if QT_VERSION < 0x050000
		qInstallMsgHandler(habitLoggingHandler);
#else
		qInstallMessageHandler(habitLoggingHandler);
#endif
	}
	qDebug() << "Starting Habit, version " << HABIT_VERSION;

	// dump process environment
	QStringList envVars = QProcessEnvironment::systemEnvironment().toStringList();
	qDebug() << "Environment: " << envVars;
	qInfo() << "Number of screens: " << QApplication::desktop()->screenCount();
	for (int i=0; i<QApplication::desktop()->screenCount(); i++)
	{
		qInfo() << "screen " << i << ": screenGeometry: " << QApplication::desktop()->screenGeometry(i);
		qInfo() << "screen " << i << ": availableGeometry: " << QApplication::desktop()->availableGeometry(i);
	}

	// dump misc dirs
	qInfo() << "DocumentsLocation: " << QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
	qInfo() << "GenericCache: " << QStandardPaths::standardLocations(QStandardPaths::GenericCacheLocation);
	qInfo() << "GenericData: " << QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);

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

