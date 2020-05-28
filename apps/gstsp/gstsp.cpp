/*
 * gstsp.cpp
 *
 *  Created on: Aug 30, 2019
 *      Author: dan
 */

#include "HMM.h"
#include <sstream>
#include <QApplication>
#include <QDir>
#include <cstdlib>
#include "HWorkspaceUtil.h"
#include "HLoggerObject.h"
#include "GstspDialog.h"
#include "GstspClientDialog.h"
#include "JsonStimFactory.h"
#include "StimDisplayWidget.h"

#include <QCameraInfo>
#include <QList>

hmm::HMM *f_pmm=NULL;
static gboolean f_bselect = false;
static gboolean f_binstalled = true;	// use -n for dev, otherwise will assume bundle is standalone
static gchar *f_pscript=NULL;
static gchar *f_pdefaultScript="../Stimuli/first.json";
static gchar *f_pworkspace=NULL;
static gchar *f_prootdir=NULL;
static gchar *f_pdefaultRootdir = "../Stimuli";
static gchar *f_pserver=NULL;
static gchar *f_pdefaultServer="127.0.0.1";
static gint f_port=5254;
static gboolean f_bfullScreen = false;
static gint f_width=320;
static gint f_height=240;
static GOptionEntry entries[] =
{
  { "server", 's', 0, G_OPTION_ARG_STRING, &f_pserver, "Server ip or name", "ip-addr" },
  { "port", 'p', 0, G_OPTION_ARG_INT, &f_port, "Server port [5254]", "port-number"},
  { "load", 'l', 0, G_OPTION_ARG_FILENAME, &f_pscript, "Script file to load on startup", "path-to-file" },
  { "workspace", 'w', 0, G_OPTION_ARG_FILENAME, &f_pworkspace, "Load experiment drop down selection from this workspace", "workspace-path" },
  { "root", 'r', 0, G_OPTION_ARG_FILENAME, &f_prootdir, "Habit stim root folder", NULL },
  { "fullscreen", 'F', 0, G_OPTION_ARG_NONE, &f_bfullScreen, "display stim fullscreen", NULL },
  { "width", 'W', 0, G_OPTION_ARG_INT, &f_width, "stim display width in pixels", "px" },
  { "height", 'H', 0, G_OPTION_ARG_INT, &f_height, "stim display height in pixels", "px" },
  { "select-workspace", 'W', 0, G_OPTION_ARG_NONE, &f_bselect, "workspace select mode", NULL },
  { "not-installed", 'N', G_OPTION_FLAG_REVERSE, G_OPTION_ARG_NONE, &f_binstalled, "developer-only, ignore bundle for libs etc.", NULL },
  { NULL }
};




gboolean handle_keyboard (GIOChannel * source, GIOCondition cond, GMainLoop *loop);

void glibPrintFunc(const gchar *string);
void glibPrinterrFunc(const gchar *string);
void gstLogFunction(GstDebugCategory * category,
                   GstDebugLevel level,
                   const gchar * file,
                   const gchar * function,
                   gint line,
                   GObject * object,
                   GstDebugMessage * message,
                   gpointer user_data);		// supposedly needs this with gnuc compiler: (void) G_GNUC_NO_INSTRUMENT

std::string getPathRelativeToExe(gchar *path);
std::string getFileRelativeToExe(gchar *path);

void gstLogFunction(GstDebugCategory * category,
                   GstDebugLevel level,
                   const gchar * file,
                   const gchar * function,
                   gint line,
                   GObject * object,
                   GstDebugMessage * message,
                   gpointer user_data)
{
	//QString s("%1 %2 %3");
	//s.arg(gst_debug_category_get_name(category)).arg(gst_debug_level_get_name(level)).arg(gst_debug_message_get(message));
	qDebug() << QString("%1 %2 %3").arg(gst_debug_category_get_name(category)).arg(gst_debug_level_get_name(level)).arg(gst_debug_message_get(message));
}


void glibPrintFunc(const gchar *string)
{
	QString s(string);
	qDebug() << s;
}

void glibPrinterrFunc(const gchar *string)
{
	QString s(string);
	qCritical() << s;
}


int main (int argc, char **argv)
{
	// dbg workaround - if arguments begin and end with single quote, remove them and continue.
	// Eclipse is adding quotes to args when running dbg. See
	// https://bugs.eclipse.org/bugs/show_bug.cgi?id=516027 (comment 5)

	for (int ii=0; ii<argc; ii++)
	{
		g_print("%d: %s\n", ii, argv[ii]);
		if (argv[ii][0] == '\'')
		{
			int l = strlen(argv[ii]);
			strncpy(argv[ii], argv[ii]+1, l-2);
			argv[ii][l-2] = 0;
		}
		g_print("%d: %s\n", ii, argv[ii]);
	}


	// parse command line
	GError *error = NULL;
	GOptionContext *context;
	context = g_option_context_new ("- remote looking time experiment platform");
	g_option_context_add_main_entries (context, entries, NULL);
	if (!g_option_context_parse (context, &argc, &argv, &error))
	{
	  g_print ("option parsing failed: %s\n", error->message);
	  exit (1);
	}

	// Qt application yakkity yak
	QApplication app(argc, argv);
	app.setApplicationName("habit2-client");
	app.setOrganizationName("Infant Cognition Lab");
	app.setOrganizationDomain("infantcognitionlab.ucdavis.edu");


	// If running as 'installed', it means we will get everything gstreamer-related from the bundle.
	// There are a handful of env variables to set.
	/*
	 * GST_PLUGIN_SYSTEM_PATH
	 * GST_PLUGIN_SCANNER
	 * GTK_PATH
	 * GIO_EXTRA_MODULES
	 *
	 */

	if (f_binstalled)
	{
		g_print("Installed, exe path %s\n", QCoreApplication::applicationDirPath().toStdString().c_str());
		g_print("PLUGINS: %s\n", boost::filesystem::path(QCoreApplication::applicationDirPath().toStdString().c_str()).append("../Frameworks/lib/gstreamer-1.0").c_str());
		setenv("GST_PLUGIN_SYSTEM_PATH", boost::filesystem::path(QCoreApplication::applicationDirPath().toStdString().c_str()).append("../Frameworks/lib/gstreamer-1.0").c_str(), 1);
		g_print("SCANNER: %s\n", boost::filesystem::path(QCoreApplication::applicationDirPath().toStdString().c_str()).append("../Frameworks/libexec/gstreamer-1.0/gst-plugin-scanner").c_str());
		setenv("GST_PLUGIN_SCANNER", boost::filesystem::path(QCoreApplication::applicationDirPath().toStdString().c_str()).append("../Frameworks/libexec/gstreamer-1.0/gst-plugin-scanner").c_str(), 1);
		g_print("gtk path: %s\n", boost::filesystem::path(QCoreApplication::applicationDirPath().toStdString().c_str()).append("../Frameworks").c_str());
		setenv("GTK_PATH", boost::filesystem::path(QCoreApplication::applicationDirPath().toStdString().c_str()).append("../Frameworks").c_str(), 1);
		g_print("gio path: %s\n", boost::filesystem::path(QCoreApplication::applicationDirPath().toStdString().c_str()).append("../Frameworks/lib/gio/modules").c_str());
		setenv("GIO_EXTRA_MODULES", boost::filesystem::path(QCoreApplication::applicationDirPath().toStdString().c_str()).append("../Frameworks/lib/gio/modules").c_str(), 1);
	}

	// init gstreamer
	gst_init(&argc, &argv);



	if (!f_bselect)
	{
		gchar *pserver = f_pdefaultServer;
		gchar *pscript = f_pdefaultScript;
		gchar *prootdir = f_pdefaultRootdir;
		std::string srootdir;	// this will be he full path
		std::string sscript;	// will be full path

		if (f_pserver) pserver = f_pserver;
		if (f_pscript) pscript = f_pscript;
		if (f_prootdir) prootdir = f_prootdir;

		g_print("Connect to server %s:%d\n", pserver, f_port);

		srootdir = getPathRelativeToExe(prootdir);
		sscript = getFileRelativeToExe(pscript);
		g_print("script arg %s use %s\n", pscript, sscript.c_str());
		g_print("image arg %s use %s\n", prootdir, srootdir.c_str());

		g_print("stim factory script %s\nstim rootdir: %s\n", sscript.c_str(), srootdir.c_str());
		hmm::JsonStimFactory *pfactory = new hmm::JsonStimFactory(sscript, srootdir);

		hmm::HMM* phmm = new hmm::HMM(pfactory->getHMMConfiguration(), *pfactory);

		// Create stimulus widget, then assign widgets to sinks for video display.
		std::vector<hmm::HMMStimPosition> vecPositions(phmm->port().getVideoPositions());
		GstspClientDialog *pClientDialog = new GstspClientDialog(pserver, f_port, phmm);
		StimDisplayWidget *psdw = new StimDisplayWidget(vecPositions.size(), true, true, 0x80808000, f_width, f_height, pClientDialog);
		for (int i=0; i<vecPositions.size(); i++)
		{
			GValue v = G_VALUE_INIT;
			g_value_init(&v, G_TYPE_POINTER);
			g_value_set_pointer(&v, psdw->getVideoWidget(i));
			phmm->port().setWidget(vecPositions[i], v);
		}

		psdw->show();
		psdw->raise();

		pClientDialog->exec();

		delete phmm;
		if (f_pserver) g_free(f_pserver);
		if (f_pscript) g_free(f_pscript);
		if (f_prootdir) g_free(f_prootdir);
	}
	else
	{
		if (f_pworkspace)
		{
			habutilSetWorkspace(QString(f_pworkspace));
			g_free(f_pworkspace);
		}
		if (f_prootdir)
		{
			habutilSetStimulusRootDir(QString(f_prootdir));
			g_free(f_prootdir);
		}

		if (!habutilInitWorkspace())
			return 0;

		GstspDialog *dialog = new GstspDialog();
		dialog->exec();
	}

	return 0;
}

std::string getPathRelativeToExe(gchar *ppath)
{
	// check if image root need image root dir. full path on command line, or relative to exe folder.
	std::string spath;
	QDir dirRoot;
	QDir dirTmp(ppath);
	if (dirTmp.isRelative())
	{
		dirRoot = QDir(QCoreApplication::applicationDirPath());
		dirRoot.cd(dirTmp.path());
		spath = dirRoot.path().toStdString();
	}
	else
	{
		spath = dirTmp.path().toStdString();
	}
	return spath;
}

std::string getFileRelativeToExe(gchar *pfile)
{
	std::string sfile;
	QDir dirRoot;
	QDir dirTmp(pfile);
	if (dirTmp.isRelative())
	{
		dirRoot = QDir(QCoreApplication::applicationDirPath());
		sfile = dirRoot.filePath(dirTmp.path()).toStdString();
	}
	else
	{
		sfile = dirTmp.path().toStdString();
	}
	return sfile;
}


//
///* Process keyboard input */
//gboolean handle_keyboard (GIOChannel * source, GIOCondition, GMainLoop *loop)
//{
//  gchar *str = NULL;
//
//  if (g_io_channel_read_line (source, &str, NULL, NULL,
//          NULL) != G_IO_STATUS_NORMAL) {
//    return TRUE;
//  }
//
//  switch (g_ascii_tolower (str[0])) {
//  case 'r':
//  {
//	  unsigned int ui = (unsigned int)atoi(str+1);
//	  try
//	  {
//		  hmm::HMMInstanceID iid = f_pmm->preroll(ui);
//		  g_print("prerolled stimid %lu got instance ID %lu\n", ui, (unsigned int)iid);
//	  }
//	  catch (std::exception const &e)
//	  {
//		  g_print("exception: %s\n", e.what());
//	  }
//	  break;
//  }
//  case 'p':
//  {
//	  unsigned int ui = (unsigned int)atoi(str+1);
//	  try
//	  {
//		  g_print("attempt to play instance ID %lu\n", ui);
//		  f_pmm->play((hmm::HMMInstanceID)ui);
//	  }
//	  catch (std::exception const &e)
//	  {
//		  g_print("exception: %s\n", e.what());
//	  }
//	  break;
//  }
//  case 's':
//  {
//	  g_print("Enter filename of src\n");
//	  break;
//  }
//  case 'd':
//  {
//	  f_pmm->dump(str);
//	  break;
//  }
//  case 'q':
//      g_main_loop_quit (loop);
//      break;
//  default:
//      break;
//  }
//
//  g_free (str);
//
//  return TRUE;
//}
//


