/*
 * gstsp.cpp
 *
 *  Created on: Aug 30, 2019
 *      Author: dan
 */

#include "HMM.h"
//#include "MM1.h"
#include <boost/filesystem.hpp>
#include <sstream>
//#include <rapidjson/>

//MM1 *f_pmm1=NULL;
hmm::HMM *f_pmm=NULL;

gboolean handle_keyboard (GIOChannel * source, GIOCondition cond, GMainLoop *loop);

int main (int argc, char **argv)
{
//  GIOChannel *io_stdin;
//  GMainLoop *loop;
  gst_init (&argc, &argv);

//  f_pmm1 = new MM1(true, false, argv[1]);

  // Set up configuration for the mm port - the backend that is - number of video outputs, audio?
  hmm::HMMConfiguration config;
  config.video[hmm::HMM::STIMPOS_CENTER] = "center";
  f_pmm = new hmm::HMM(config);

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


/* Process keyboard input */
gboolean handle_keyboard (GIOChannel * source, GIOCondition, GMainLoop *loop)
{
  gchar *str = NULL;

  if (g_io_channel_read_line (source, &str, NULL, NULL,
          NULL) != G_IO_STATUS_NORMAL) {
    return TRUE;
  }

  switch (g_ascii_tolower (str[0])) {
  case 'r':
  {
	  unsigned int ui = (unsigned int)atoi(str+1);
	  try
	  {
		  hmm::HMMInstanceID iid = f_pmm->preroll(ui);
		  g_print("prerolled stimid %lu got instance ID %lu\n", ui, (unsigned int)iid);
	  }
	  catch (std::exception const &e)
	  {
		  g_print("exception: %s\n", e.what());
	  }
	  break;
  }
  case 'p':
  {
	  unsigned int ui = (unsigned int)atoi(str+1);
	  try
	  {
		  g_print("attempt to play instance ID %lu\n", ui);
		  f_pmm->play((hmm::HMMInstanceID)ui);
	  }
	  catch (std::exception const &e)
	  {
		  g_print("exception: %s\n", e.what());
	  }
	  break;
  }
  case 's':
  {
	  g_print("Enter filename of src\n");
	  break;
  }
  case 'd':
  {
	  f_pmm->dump(str);
	  break;
  }
  case 'q':
      g_main_loop_quit (loop);
      break;
  default:
      break;
  }

  g_free (str);

  return TRUE;
}


#if 0

#include <gst/gst.h>
#include <stdio.h>
#include <vector>

typedef struct
{
	bool prerolling;
	bool prerolled;
	bool playing;
	bool done;
	GString *filename;
	GstElement *filesrc;
	GstElement *dbin;
	GstPad *vsrcpad;
	GstPad *asrcpad;
	bool haveV, haveA;
	bool vblocked, ablocked;
	gulong probeidV, probeidA;
} Stim;


typedef struct
{
	std::vector<Stim *> *pstimuli;
	GMainLoop *loop;
	GstElement *pipeline;
	GstElement *testsrc;
} CustomData;

static GMainLoop *loop;
static GstElement *pipeline;
static GstElement *conv, *scale, *avsink;
std::vector<Stim *> srcs;
static Stim *newstim;
static Stim *currentstim;


void prerollStim(Stim *stim, GstElement *pipeline);
static gboolean message_cb (GstBus * bus, GstMessage * message, gpointer user_data);
static gboolean handle_keyboard (GIOChannel * source, GIOCondition cond, CustomData *data);
void pad_added_cb (GstElement * element, GstPad * pad, gpointer user_data);
GstPadProbeReturn pad_probe_block_cb (GstPad * pad, GstPadProbeInfo * info, gpointer user_data);
GstPadProbeReturn pad_probe_block_for_switch_cb (GstPad * pad, GstPadProbeInfo * info, gpointer user_data);

int main (int argc, char **argv)
{
  GstBus *bus;
  GIOChannel *io_stdin;
  CustomData data;

  gst_init (&argc, &argv);

  if (argc < 2) {
    g_error ("Usage: %s filename [filename2 [...]]", argv[0]);
    return -1;
  }

  // make the tail end of the pipeline
  // the src part will be linked/unlinked
  pipeline = gst_pipeline_new (NULL);
  conv = gst_element_factory_make ("videoconvert", NULL);
  scale = gst_element_factory_make ("videoscale", NULL);
  // works on mac
  avsink = gst_element_factory_make("autovideosink", NULL);
  // works on linux avsink = gst_element_factory_make("xvimagesink", NULL);

  // make test src
  data.testsrc = gst_element_factory_make("videotestsrc", NULL);
  gst_object_ref(data.testsrc);


  if (!pipeline || !conv || !scale || !avsink || !data.testsrc)
  {
	  g_error ("Failed to create elements");
	  return -1;
  }
  gst_bin_add_many (GST_BIN (pipeline), data.testsrc, conv, scale, avsink, NULL);
  if (!gst_element_link_many (data.testsrc, conv, scale, avsink, NULL))
  {
	  g_error ("Failed to link elements");
	  return -2;
  }
  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  Stim *stim = g_new0(Stim, 1);
  stim->filename = g_string_new("testsrc");
  stim->prerolling = stim->prerolled = stim->done = false;
  stim->playing = true;
  stim->haveV = true;
  stim->haveA = false;
  stim->vblocked  = stim->ablocked = false;
  stim->vsrcpad = gst_element_get_static_pad(data.testsrc, "src");
  gst_object_ref(data.testsrc);

  srcs.push_back(stim);
  currentstim = stim;


  for (int i=1; i<argc; i++)
  {
	  Stim *stim = g_new0(Stim, 1);
	  stim->filename = g_string_new(argv[i]);
	  stim->prerolling = stim->prerolled = stim->playing = stim->done = false;
	  stim->haveV = stim->haveA = false;
	  stim->vblocked  = stim->ablocked = false;
	  srcs.push_back(stim);
	  g_print("File %d: %s\n", i, stim->filename->str);
  }

  g_print("create main loop\n");
  loop = g_main_loop_new (NULL, FALSE);

  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  gst_bus_add_signal_watch (bus);
  g_signal_connect (G_OBJECT (bus), "message", G_CALLBACK (message_cb), &data);
  gst_object_unref (GST_OBJECT (bus));


  /* Add a keyboard watch so we get notified of keystrokes */
  data.pstimuli = &srcs;
  data.pipeline = pipeline;
  data.loop = loop;
#ifdef G_OS_WIN32
  io_stdin = g_io_channel_win32_new_fd (fileno (stdin));
#else
  io_stdin = g_io_channel_unix_new (fileno (stdin));
#endif
  g_io_add_watch (io_stdin, G_IO_IN, (GIOFunc) handle_keyboard, &data);

  g_print("run loop\n");
  g_main_loop_run (loop);

  gst_element_set_state (pipeline, GST_STATE_NULL);

  g_main_loop_unref (loop);

  gst_object_unref (pipeline);

  g_print("quitting\n");

  return 0;
}



void playStim(CustomData *pdata)
{
	// set blocking probes on src pad(s) of current stim.
	// Block all sources, the callback will count and decide when there's enough blockage.
	newstim = NULL;
	for (auto s : *pdata->pstimuli)
	{
		if (!s->done && !s->playing && s->prerolled)
		{
			g_print("available to play %s\n", s->filename->str);
			newstim = s;
			break;
		}
	}

	if (newstim)
	{
		// set blocking probe(s)
		currentstim->probeidV = gst_pad_add_probe (currentstim->vsrcpad, GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, pad_probe_block_for_switch_cb, currentstim, NULL);

	}
}

void prerollStim(Stim *stim, GstElement *pipeline)
{
	g_print("start preroll for stim file %s\n", stim->filename->str);

	stim->filesrc = gst_element_factory_make ("filesrc", NULL);
	stim->dbin = gst_element_factory_make ("decodebin", NULL);
	g_object_set (stim->filesrc, "location", stim->filename->str, NULL);

	gst_object_ref(stim->filesrc);
	gst_object_ref(stim->dbin);

	gst_bin_add_many (GST_BIN (pipeline), stim->filesrc, stim->dbin, NULL);
	if (!gst_element_link_many (stim->filesrc, stim->dbin, NULL))
	{
	  g_error ("Failed to link elements");
	}

	g_signal_connect (stim->dbin, "pad-added", G_CALLBACK (pad_added_cb), stim);

    gst_element_sync_state_with_parent (stim->filesrc);
    gst_element_sync_state_with_parent (stim->dbin);
}


static gboolean
message_cb (GstBus * bus, GstMessage * message, gpointer user_data)
{
	CustomData* pdata = (CustomData *)user_data;
	switch (GST_MESSAGE_TYPE (message))
	{
	case GST_MESSAGE_ERROR:
	{
		GError *err = NULL;
		gchar *name, *debug = NULL;

		name = gst_object_get_path_string (message->src);
		gst_message_parse_error (message, &err, &debug);

		g_printerr ("ERROR: from element %s: %s\n", name, err->message);
		if (debug != NULL)
		g_printerr ("Additional debug info:\n%s\n", debug);

		g_error_free (err);
		g_free (debug);
		g_free (name);

		g_main_loop_quit (loop);
		break;
	}
	case GST_MESSAGE_WARNING:
	{
		GError *err = NULL;
		gchar *name, *debug = NULL;

		name = gst_object_get_path_string (message->src);
		gst_message_parse_warning (message, &err, &debug);

		g_printerr ("ERROR: from element %s: %s\n", name, err->message);
		if (debug != NULL)
		g_printerr ("Additional debug info:\n%s\n", debug);

		g_error_free (err);
		g_free (debug);
		g_free (name);
		break;
	}
	case GST_MESSAGE_EOS:
	{
		g_print ("Got EOS\n");
		g_main_loop_quit (loop);
		break;
	}
	case GST_MESSAGE_APPLICATION:
	{
		if (gst_message_has_name (message, "Switch"))
		{
			/* it's our message */
			g_print ("Ready to do switch\n");

			// detach vsrcpad newstim->vsrcpad from conv sink pad
			GstPad *convsink = gst_element_get_static_pad(conv, "sink");
			gst_pad_unlink(currentstim->vsrcpad, convsink);

			// get rid of test src
			gst_element_set_state(pdata->testsrc, GST_STATE_NULL);

			// link new src and set offset
			gst_pad_link(newstim->vsrcpad, convsink);

			GstClockTime abs = gst_clock_get_time(gst_element_get_clock(conv));
			GstClockTime base = gst_element_get_base_time(conv);
			// same base time GstClockTime base2 = gst_element_get_base_time(newstim->dbin);
			// TRY SETTING ON SINK gst_pad_set_offset(newstim->vsrcpad, abs-base);
			gst_pad_set_offset(convsink, abs-base);
			gst_object_unref(convsink);

			g_print("GST_MESSAGE_APPLICATION:  abs %u base %u run time %u\n", abs, base, GST_TIME_AS_MSECONDS(abs-base));


			// unblock vsrcpad
			gst_pad_remove_probe(newstim->vsrcpad, newstim->probeidV);
			newstim->vblocked = false;
		}
		break;
	}
	default:
		break;
	}

	return TRUE;
}


/* Process keyboard input */
static gboolean
handle_keyboard (GIOChannel * source, GIOCondition cond, CustomData *pdata)
{
  gchar *str = NULL;

  if (g_io_channel_read_line (source, &str, NULL, NULL,
          NULL) != G_IO_STATUS_NORMAL) {
    return TRUE;
  }

  g_print("keyboard input: %s\n", str);

  switch (g_ascii_tolower (str[0])) {
  case 'p':
	  for (auto s : *pdata->pstimuli)
	  {
		  if (!s->playing && !s->prerolled && !s->prerolling)
		  {
			  g_print("available to preroll %s\n", s->filename->str);
			  s->prerolling =  true;
			  prerollStim(s, pdata->pipeline);
			  break;
		  }
	  }
	  break;
  case 's':
  {
	  g_print("Ready to swap new src\n");
	  playStim(pdata);
	  break;
  }
  case 'q':
      g_main_loop_quit (pdata->loop);
      break;
  case 'd':
	  g_print("dump to dot file\n");
	  GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "gstsp");
	  break;
  default:
      break;
  }

  g_free (str);

  return TRUE;
}

void pad_added_cb (GstElement * element, GstPad * pad, gpointer user_data)
{
	GstCaps *caps;
	GstStructure *s;
	const gchar *name;
	Stim *stim = (Stim *)user_data;

	caps = gst_pad_get_current_caps (pad);
	s = gst_caps_get_structure (caps, 0);
	name = gst_structure_get_name (s);

	g_print("pad_added_cb: %s\n", name);
	if (strcmp (name, "video/x-raw") == 0)
	{
		// block pad
		stim->vsrcpad = pad;
		stim->haveV = true;
		gst_object_ref(stim->vsrcpad);
		stim->probeidV = gst_pad_add_probe (stim->vsrcpad, GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, pad_probe_block_cb, user_data, NULL);
	}

	gst_caps_unref (caps);
}

GstPadProbeReturn pad_probe_block_cb (GstPad * pad, GstPadProbeInfo * info, gpointer user_data)
{
  Stim *stim = (Stim *)user_data;
  if (stim->haveA && pad==stim->asrcpad)
  {
	  stim->ablocked = true;
	  g_print("audio blocked\n");
  }
  else if (stim->haveV && pad==stim->vsrcpad)
  {
	  stim->vblocked = true;
	  g_print("video blocked\n");
  }

  if (		((stim->haveA && stim->ablocked) || !stim->haveA) &&
		  	((stim->haveV && stim->vblocked) || !stim->haveV))
  {
	  g_print("prerolled\n");
	  stim->prerolled = true;
	  stim->prerolling = false;
  }
  return GST_PAD_PROBE_OK;
}



GstPadProbeReturn pad_probe_block_for_switch_cb (GstPad * pad, GstPadProbeInfo * info, gpointer user_data)
{
	Stim *stim = (Stim *)user_data;
	if (stim->haveA && pad==stim->asrcpad)
	{
	  stim->ablocked = true;
	  g_print("audio blocked for switch\n");
	}
	else if (stim->haveV && pad==stim->vsrcpad)
	{
	  stim->vblocked = true;
	  g_print("video blocked for switch\n");
	}

	// check if all src pads blocked; if so, send bus msg
	if (	((stim->haveA && stim->ablocked) || !stim->haveA) &&
			((stim->haveV && stim->vblocked) || !stim->haveV))
	{
		g_print("Current source is blocked, send msg\n");
		GstBus *bus;
		bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
		gst_bus_post (bus, gst_message_new_application(GST_OBJECT_CAST (pipeline), gst_structure_new_empty ("Switch")));
		gst_object_unref (GST_OBJECT (bus));
	}

	return GST_PAD_PROBE_OK;
}

#endif
