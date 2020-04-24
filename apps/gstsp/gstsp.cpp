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
#include "HWorkspaceUtil.h"
#include "HLoggerObject.h"
#include "GstspDialog.h"
#include "GstspClientDialog.h"
#include "JsonStimFactory.h"
#include "StimDisplayWidget.h"

#include <QCameraInfo>
#include <QList>

hmm::HMM *f_pmm=NULL;
static gchar *f_script=NULL;
static gchar *f_workspace=NULL;
static gchar *f_rootdir=NULL;
static gchar *f_server=NULL;
static gchar *f_defaultServer="127.0.0.1";
static gint f_port=5254;
static gboolean f_bfullScreen = false;
static gint f_width=320;
static gint f_height=240;
static GOptionEntry entries[] =
{
  { "server", 's', 0, G_OPTION_ARG_STRING, &f_server, "Server ip or name", "ip-addr" },
  { "port", 'p', 0, G_OPTION_ARG_INT, &f_port, "Server port [5254]", "port-number"},
  { "load", 'l', 0, G_OPTION_ARG_FILENAME, &f_script, "Script file to load on startup", "path-to-file" },
  { "workspace", 'w', 0, G_OPTION_ARG_FILENAME, &f_workspace, "Load experiment drop down selection from this workspace", "workspace-path" },
  { "root", 'r', 0, G_OPTION_ARG_FILENAME, &f_rootdir, "Habit stim root folder", NULL },
  { "fullscreen", 'F', 0, G_OPTION_ARG_NONE, &f_bfullScreen, "display stim fullscreen", NULL },
  { "width", 'W', 0, G_OPTION_ARG_INT, &f_width, "stim display width in pixels", "px" },
  { "height", 'H', 0, G_OPTION_ARG_INT, &f_height, "stim display height in pixels", "px" },
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

	// init gstreamer
	gst_init(&argc, &argv);


	if (f_server)
	{
		if (!f_script)
		{
			g_print("must specify script file on command line\n");
			exit(1);
		}

		g_print("Server option %s : %d\n", f_server, f_port);

		// need image root dir. full path on command line, or relative to exe folder.
		std::string imageroot;
		if (f_rootdir)
		{
			g_print("rootdir %s\n", f_rootdir);
			imageroot = std::string(f_rootdir);
			g_free(f_rootdir);
		}
		else
		{
			QDir dirExe(QCoreApplication::applicationDirPath());
			dirExe.cd("../../Stimuli");
			imageroot = dirExe.path().toStdString();
		}

		hmm::JsonStimFactory *pfactory = new hmm::JsonStimFactory(std::string(f_script), imageroot);

		hmm::HMM* phmm = new hmm::HMM(pfactory->getHMMConfiguration(), *pfactory);

		// Create stimulus widget, then assign widgets to sinks for video display.
		std::vector<hmm::HMMStimPosition> vecPositions(phmm->port().getVideoPositions());
		GstspClientDialog *pClientDialog = new GstspClientDialog(f_server, f_port);
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
		g_free(f_server);
		g_free(f_script);
	}
	else
	{
		if (f_workspace)
		{
			habutilSetWorkspace(QString(f_workspace));
			g_free(f_workspace);
		}
		if (f_rootdir)
		{
			habutilSetStimulusRootDir(QString(f_rootdir));
			g_free(f_rootdir);
		}

		if (!habutilInitWorkspace())
			return 0;

		GstspDialog *dialog = new GstspDialog();
		dialog->exec();
	}

	return 0;
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


