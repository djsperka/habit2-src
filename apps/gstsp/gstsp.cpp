/*
 * gstsp.cpp
 *
 *  Created on: Aug 30, 2019
 *      Author: dan
 */

#include "HMM.h"
#include <boost/filesystem.hpp>
#include <sstream>
#include <QApplication>
#include "HWorkspaceUtil.h"
#include "HLoggerObject.h"
#include "GstspDialog.h"



//MM1 *f_pmm1=NULL;
hmm::HMM *f_pmm=NULL;

gboolean handle_keyboard (GIOChannel * source, GIOCondition cond, GMainLoop *loop);

int main (int argc, char **argv)
{
	QApplication app(argc, argv);
	gst_init(&argc, &argv);
	app.setApplicationName("habit2");
	app.setOrganizationName("Infant Cognition Lab");
	app.setOrganizationDomain("infantcognitionlab.ucdavis.edu");

	// specify workspace dir and/or stim root dir
	for (int i=1; i<argc; i++)
	{
		if (!strcmp(argv[i], "-w"))
		{
			if (i < (argc-1))
			{
				habutilSetWorkspace(argv[i+1]);
				i++;
			}
		}
		else if (!strcmp(argv[i], "-r"))
		{
			habutilSetStimulusRootDir(QString(argv[i+1]));
			i++;
		}
		else
		{
			qDebug() << "Unrecognized command line arg " << QString(argv[i]);
		}
	}

	qInstallMessageHandler(&HLoggerObject::loggingHandler);

	if (!habutilInitWorkspace())
		return 0;

	GstspDialog *dialog = new GstspDialog();
	dialog->exec();

//  // Set up configuration for the mm port - the backend that is - number of video outputs, audio?
//  hmm::HMMConfiguration config;
//  config.video[hmm::HMM::STIMPOS_CENTER] = "center";
//  f_pmm = new hmm::HMM(config);

//  g_print("create main loop\n");
//  loop = g_main_loop_new (NULL, FALSE);


//  /* Add a keyboard watch so we get notified of keystrokes */
//#ifdef G_OS_WIN32
//  io_stdin = g_io_channel_win32_new_fd (fileno (stdin));
//#else
//  io_stdin = g_io_channel_unix_new (fileno (stdin));
//#endif
//  g_io_add_watch (io_stdin, G_IO_IN, (GIOFunc) handle_keyboard, loop);
//
//  g_print("run loop\n");
//  g_main_loop_run (loop);
//
//  g_main_loop_unref (loop);
//  g_print("quitting\n");

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


