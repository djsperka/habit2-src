#include <string.h>
#include <gst/gst.h>

typedef struct
{
	GString *filename;
	GstElement *filesrc;
	GstElement *dbin;
	GstPad *srcpad;
} SourceElements;

static GMainLoop *loop;
static GstElement *pipeline;
static GstElement *conv, *scale, *avsink;
static gboolean linked = FALSE;
static GList *srcs;
static int state=0;
static gint in_idle_probe = FALSE;

static gboolean tick_cb (gpointer data);
static gboolean message_cb (GstBus * bus, GstMessage * message, gpointer user_data);
static void pad_probe_block_cb (GstElement * element, GstPad * pad, gpointer user_data);
static GstPadProbeReturn pad_probe_block_cb (GstPad * pad, GstPadProbeInfo * info, gpointer user_data);
static GstPadProbeReturn pad_probe_idle_cb (GstPad * pad, GstPadProbeInfo * info, gpointer user_data);
static void pad_added_cb (GstElement * element, GstPad * pad, gpointer user_data);
static void pad_added_preroll_cb (GstElement * element, GstPad * pad, gpointer user_data);
static void no_more_pads_preroll_cb (GstElement * element, gpointer user_data);

static gboolean tick_cb (gpointer data)
{
	SourceElements *se;
	GstPad *sinkpad;


	// get running time
	GstClockTime abs = gst_clock_get_time(gst_element_get_clock(pipeline));
	GstClockTime base = gst_element_get_base_time(pipeline);

	g_print("tick_cb state is %d, abs %u base %u run time %u\n", state, abs, base, GST_TIME_AS_SECONDS(abs-base));

	// check state
	// 0 - initial
	// 1 - set when pipeline initially starts playing
	// 2 - set here to force one cycle of running before preroll test
	// 3 - preroll test here

	switch(state)
	{
	case 0:
		break;
	case 1:
		g_atomic_int_set(&state, 2);
		break;
	case 2:
		g_print("do preroll test here\n");
		// check if there's another source in the list...
		if (srcs->next != NULL)
		{
			SourceElements *sele1 = (SourceElements *)srcs->next->data;
			g_print("next file is %s\n", sele1->filename->str);
			sele1->filesrc = gst_element_factory_make ("filesrc", NULL);
			sele1->dbin = gst_element_factory_make ("decodebin", NULL);
			g_object_ref(sele1->filesrc);
			g_object_ref(sele1->dbin);
			g_object_set (sele1->filesrc, "location", sele1->filename->str, NULL);
			gst_bin_add_many (GST_BIN (pipeline), sele1->filesrc, sele1->dbin, NULL);
			gst_element_link_many (sele1->filesrc, sele1->dbin, NULL);

			// set pad-added listener1
			g_signal_connect (sele1->dbin, "pad-added", G_CALLBACK (pad_added_preroll_cb), sele1);
			g_signal_connect(sele1->dbin, "no-more-pads", G_CALLBACK(no_more_pads_preroll_cb), sele1);
			gst_element_sync_state_with_parent (sele1->dbin);
			gst_element_sync_state_with_parent (sele1->filesrc);


		}
		g_atomic_int_set(&state, 3);
		break;
	case 4:
		g_atomic_int_set(&state, 5);
		break;
	case 5:
		g_print("at state 5, set idle probe\n");
		// set idle probe on converter sink pad, not on dbin src -- latter will cause crash in idle probe
		// when dbin is removed/state changed. Note that userdata sent is the SourceElements obj for the
		// piece that will be destroyed, thus the probe will have filesrc, dbin, and the dbin srcpad.
		in_idle_probe = FALSE;
		se = (SourceElements *)srcs->data;
		sinkpad = gst_element_get_static_pad(conv, "sink");
		gst_pad_add_probe (sinkpad, GST_PAD_PROBE_TYPE_IDLE, pad_probe_idle_cb, se, NULL);
		gst_object_unref(sinkpad);
		break;
	case 6:
		g_print("set to 7 duh\n");
		g_atomic_int_set(&state, 7);
		break;
	case 7:
		GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "after");
		g_atomic_int_set(&state, 8);
		break;
	default:
		break;
	}
	return TRUE;
}

static GstPadProbeReturn
cb_have_data (GstPad          *pad,
              GstPadProbeInfo *info,
              gpointer         user_data)
{
  GstBuffer *buffer;
  buffer = GST_PAD_PROBE_INFO_BUFFER (info);
  g_print("have data\n");

  return GST_PAD_PROBE_OK;
}


static gboolean message_cb (GstBus * bus, GstMessage * message, gpointer user_data)
{
  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR:{
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
    case GST_MESSAGE_WARNING:{
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
    case GST_MESSAGE_STATE_CHANGED:
    {
        GstState old_state, new_state;
        gst_message_parse_state_changed (message, &old_state, &new_state, NULL);
        if (g_strcmp0(GST_OBJECT_NAME (message->src), "pipeline0") &&
        		old_state == GST_STATE_PAUSED && new_state == GST_STATE_PLAYING)
        {
        	g_atomic_int_compare_and_exchange (&state, 0, 1);
        }
    	break;
    }
    case GST_MESSAGE_EOS:
      g_print ("Got EOS\n");
      g_main_loop_quit (loop);
      break;
    default:
      break;
  }

  return TRUE;
}


// The first step in replacing the head of the pipeline is to set an idle probe at the point where
// we want to break off the head. This probe is set on the pad that will be kept, on the videoconverter
// element's sink pad, not the decodebin src pad on the head-side.
// (This is because the probe has to remove and destroy elements, but this pad is part of one of those
// elements, which leads to bad things and a crash).
// Inside the probe,

static GstPadProbeReturn pad_probe_idle_cb (GstPad * pad, GstPadProbeInfo * info, gpointer user_data)
{
	// check if we're in this func already
	if (!g_atomic_int_compare_and_exchange (&in_idle_probe, FALSE, TRUE))
		return GST_PAD_PROBE_OK;

	GstPad *srcpad, *sinkpad;
	SourceElements *sele = (SourceElements *)user_data;

	//GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, GST_ELEMENT_NAME(pipeline));

	GstElement *parent, *dbinparent;
	parent = gst_pad_get_parent_element(pad);
	dbinparent = gst_pad_get_parent_element(sele->srcpad);
	gchar *parentname, *dbinparentname;
	parentname = gst_element_get_name(parent);
	dbinparentname = gst_element_get_name(dbinparent);
	g_print("pad_probe_idle_cb unlink dbin srcpad on %s from sinkpad on %s\n", dbinparentname, parentname);
	g_free(parentname);
	g_free(dbinparentname);	// what a pain
	gst_object_unref(dbinparent);
	gst_object_unref(parent);

	// get running time
	GstClockTime abs = gst_clock_get_time(gst_element_get_clock(sele->dbin));
	GstClockTime base = gst_element_get_base_time(sele->dbin);

	// unlink decodebin from tail
	if (!gst_pad_unlink(sele->srcpad, pad))
	{
		g_error("cannot unlink srcpad and idle probe pad\n");
	}

	// now unlink filesrc and decodebin
	g_print("pad_probe_idle_cb unlink filesrc from dbin\n");
	srcpad = gst_element_get_static_pad(sele->filesrc, "src");
	sinkpad = gst_element_get_static_pad(sele->dbin, "sink");
	gst_pad_unlink(srcpad, sinkpad);
	gst_object_unref(srcpad);
	gst_object_unref(sinkpad);

	g_print("pad_probe_idle_cb remove filesrc and dbin from pipeline\n");
	gst_bin_remove(GST_BIN(pipeline), sele->filesrc);
	gst_bin_remove(GST_BIN(pipeline), sele->dbin);

	g_print("pad_probe_idle_cb set filesrc, dbin NULL\n");
	gst_element_set_state(sele->dbin, GST_STATE_NULL);
	gst_element_set_state(sele->filesrc, GST_STATE_NULL);

	gst_object_unref(sele->filesrc);
	gst_object_unref(sele->dbin);


	// hack - must get src pad from next element
	g_print("pad_probe_idle_cb link new src NULL\n");
//	sinkpad = gst_element_get_static_pad (conv, "sink");
	SourceElements *sele1 = (SourceElements *)srcs->next->data;
	if (GST_PAD_LINK_FAILED(gst_pad_link(sele1->srcpad, pad)))
	{
		g_printerr("cannot link new srcpad with probe pad\n");
	}
	// absolute time gst_clock_get_time ()
	// running-time = absolute-time - base-time
	gst_pad_set_offset(sele1->srcpad, abs-base);
	//gst_object_unref(sinkpad);

	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	g_print("set to 6\n");
	g_atomic_int_set(&state, 7);

	// add data probe
	gst_pad_add_probe (pad, GST_PAD_PROBE_TYPE_BUFFER,
	      (GstPadProbeCallback) cb_have_data, NULL, NULL);



	g_print("pad_probe_idle_cb return\n");
	return GST_PAD_PROBE_REMOVE;
}

static GstPadProbeReturn pad_probe_block_cb (GstPad * pad, GstPadProbeInfo * info, gpointer user_data)
{
  SourceElements *sele = (SourceElements *)user_data;
  g_print("pad_probe_block_cb\n");
  g_atomic_int_set(&state, 4);
  return GST_PAD_PROBE_OK;
}

static void no_more_pads_preroll_cb (GstElement * element, gpointer user_data)
{
	g_print("no more pads\n");
	SourceElements *sele = (SourceElements *)user_data;
    gst_pad_add_probe (sele->srcpad, GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, pad_probe_block_cb, user_data, NULL);
}


// pad-added calback for preroll elements
static void pad_added_preroll_cb (GstElement * element, GstPad * pad, gpointer user_data)
{
  GstCaps *caps;
  GstStructure *s;
  const gchar *name;
  SourceElements *sele = (SourceElements *)user_data;

  caps = gst_pad_get_current_caps (pad);
  s = gst_caps_get_structure (caps, 0);
  name = gst_structure_get_name (s);

  g_print("pad_added_preroll_cb name %s\n", name);
  if (strcmp (name, "video/x-raw") == 0)
  {
    sele->srcpad = pad;
    g_object_ref(sele->srcpad);
  }

  gst_caps_unref (caps);
}



static void pad_added_cb (GstElement * element, GstPad * pad, gpointer user_data)
{
  GstCaps *caps;
  GstStructure *s;
  const gchar *name;
  SourceElements *sele = (SourceElements *)user_data;

  caps = gst_pad_get_current_caps (pad);
  s = gst_caps_get_structure (caps, 0);
  name = gst_structure_get_name (s);

  g_print("pad_added_cb\n");
  if (strcmp (name, "video/x-raw") == 0)
  {
    GstPad *sinkpad, *teepad;
    GstPadTemplate *templ;

    g_print("pad_added_cb - video/x-raw\n");
    sinkpad = gst_element_get_static_pad (conv, "sink");
    if (gst_pad_link (pad, sinkpad) != GST_PAD_LINK_OK) {
      g_printerr ("Failed to link dbin with conv\n");
      gst_object_unref (sinkpad);
      g_main_loop_quit (loop);
      return;
    }
    sele->srcpad = pad;
    gst_object_unref (sinkpad);

    g_timeout_add_seconds (1, tick_cb, NULL);

  }

  gst_caps_unref (caps);
}



int main (int argc, char **argv)
{
  GstBus *bus;

  gst_init (&argc, &argv);

  if (argc < 2) {
    g_error ("Usage: %s filename [filename2 [...]]", argv[0]);
    return -1;
  }

  for (int i=1; i<argc; i++)
  {
	  SourceElements *sele = g_new0(SourceElements, 1);
	  sele->filename = g_string_new(argv[i]);
	  srcs = g_list_append(srcs, sele);	// automatically creates if null?
	  g_print("File %d: %s\n", i, sele->filename->str);
  }


  // make the tail end of the pipeline
  // the src part will be linked/unlinked
  pipeline = gst_pipeline_new (NULL);
  conv = gst_element_factory_make ("videoconvert", NULL);
  scale = gst_element_factory_make ("videoscale", NULL);
  // works on mac
  avsink = gst_element_factory_make("autovideosink", NULL);
  // works on linux avsink = gst_element_factory_make("xvimagesink", NULL);
  if (!pipeline || !conv || !scale || !avsink)
  {
	  g_error ("Failed to create elements");
	  return -1;
  }
  gst_bin_add_many (GST_BIN (pipeline), conv, scale, avsink, NULL);
  if (!gst_element_link_many (conv, scale, avsink, NULL))
  {
	  g_error ("Failed to link elements");
	  return -2;
  }


  // now make the first src part, add it, link it, and set playing
  g_print("make first src\n");
  SourceElements *sele0 = (SourceElements *)srcs->data;
  sele0->filesrc = gst_element_factory_make ("filesrc", NULL);
  sele0->dbin = gst_element_factory_make ("decodebin", NULL);
  g_object_ref(sele0->filesrc);
  g_object_ref(sele0->dbin);
  g_object_set (sele0->filesrc, "location", sele0->filename->str, NULL);

  gst_bin_add_many (GST_BIN (pipeline), sele0->filesrc, sele0->dbin, NULL);
  if (!gst_element_link_many (sele0->filesrc, sele0->dbin, NULL))
  {
	  g_error ("Failed to link elements");
	  return -2;
  }

  g_signal_connect (sele0->dbin, "pad-added", G_CALLBACK (pad_added_cb), sele0);

  g_print("create main loop\n");
  loop = g_main_loop_new (NULL, FALSE);

  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  gst_bus_add_signal_watch (bus);
  g_signal_connect (G_OBJECT (bus), "message", G_CALLBACK (message_cb), NULL);
  gst_object_unref (GST_OBJECT (bus));

  g_print("play\n");
  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  g_print("run loop\n");
  g_main_loop_run (loop);

  gst_element_set_state (pipeline, GST_STATE_NULL);

  g_main_loop_unref (loop);

  gst_object_unref (pipeline);





  return 0;
}

#ifdef EXAMPLE2

static GMainLoop *loop;
static GstElement *pipeline;
static GstElement *src, *dbin, *conv, *tee;
static gboolean linked = FALSE;
static GList *sinks;

typedef struct
{
  GstPad *teepad;
  GstElement *queue;
  GstElement *conv;
  GstElement *sink;
  gboolean removing;
} Sink;

static gboolean
message_cb (GstBus * bus, GstMessage * message, gpointer user_data)
{
  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR:{
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
    case GST_MESSAGE_WARNING:{
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
      g_print ("Got EOS\n");
      g_main_loop_quit (loop);
      break;
    default:
      break;
  }

  return TRUE;
}

static GstPadProbeReturn
unlink_cb (GstPad * pad, GstPadProbeInfo * info, gpointer user_data)
{
  Sink *sink = (Sink *)user_data;
  GstPad *sinkpad;

  if (!g_atomic_int_compare_and_exchange (&sink->removing, FALSE, TRUE))
    return GST_PAD_PROBE_OK;

  sinkpad = gst_element_get_static_pad (sink->queue, "sink");
  gst_pad_unlink (sink->teepad, sinkpad);
  gst_object_unref (sinkpad);

  gst_bin_remove (GST_BIN (pipeline), sink->queue);
  gst_bin_remove (GST_BIN (pipeline), sink->conv);
  gst_bin_remove (GST_BIN (pipeline), sink->sink);

  gst_element_set_state (sink->sink, GST_STATE_NULL);
  gst_element_set_state (sink->conv, GST_STATE_NULL);
  gst_element_set_state (sink->queue, GST_STATE_NULL);

  gst_object_unref (sink->queue);
  gst_object_unref (sink->conv);
  gst_object_unref (sink->sink);

  gst_element_release_request_pad (tee, sink->teepad);
  gst_object_unref (sink->teepad);

  g_print ("removed\n");

  return GST_PAD_PROBE_REMOVE;
}

static gboolean
tick_cb (gpointer data)
{
  if (!sinks || g_random_int () % 2 == 0) {
    Sink *sink = g_new0 (Sink, 1);
    GstPad *sinkpad;
    GstPadTemplate *templ;

    templ =
        gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (tee),
        "src_%u");

    g_print ("add\n");

    sink->teepad = gst_element_request_pad (tee, templ, NULL, NULL);

    sink->queue = gst_element_factory_make ("queue", NULL);
    sink->conv = gst_element_factory_make ("videoconvert", NULL);
    sink->sink = gst_element_factory_make ("autovideosink", NULL);
    sink->removing = FALSE;

    gst_object_ref (sink->queue);
    gst_object_ref (sink->conv);
    gst_object_ref (sink->sink);
    gst_bin_add_many (GST_BIN (pipeline), sink->queue, sink->conv, sink->sink, NULL);
    gst_element_link_many (sink->queue, sink->conv, sink->sink, NULL);

    gst_element_sync_state_with_parent (sink->queue);
    gst_element_sync_state_with_parent (sink->conv);
    gst_element_sync_state_with_parent (sink->sink);

    sinkpad = gst_element_get_static_pad (sink->queue, "sink");
    gst_pad_link (sink->teepad, sinkpad);
    gst_object_unref (sinkpad);

    g_print ("added\n");

    sinks = g_list_append (sinks, sink);
  } else {
    Sink *sink;

    g_print ("remove\n");

    sink = (Sink *)sinks->data;
    sinks = g_list_delete_link (sinks, sinks);
    gst_pad_add_probe (sink->teepad, GST_PAD_PROBE_TYPE_IDLE, unlink_cb, sink,
        (GDestroyNotify) g_free);
  }

  return TRUE;
}

static void
pad_added_cb (GstElement * element, GstPad * pad, gpointer user_data)
{
  GstCaps *caps;
  GstStructure *s;
  const gchar *name;

  if (linked)
    return;

  caps = gst_pad_get_current_caps (pad);
  s = gst_caps_get_structure (caps, 0);
  name = gst_structure_get_name (s);

  if (strcmp (name, "video/x-raw") == 0) {
    GstPad *sinkpad, *teepad;
    GstElement *queue, *sink;
    GstPadTemplate *templ;

    sinkpad = gst_element_get_static_pad (conv, "sink");
    if (gst_pad_link (pad, sinkpad) != GST_PAD_LINK_OK) {
      g_printerr ("Failed to link dbin with conv\n");
      gst_object_unref (sinkpad);
      g_main_loop_quit (loop);
      return;
    }
    gst_object_unref (sinkpad);

    templ =
        gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (tee),
        "src_%u");
    teepad = gst_element_request_pad (tee, templ, NULL, NULL);
    queue = gst_element_factory_make ("queue", NULL);
    sink = gst_element_factory_make ("fakesink", NULL);

    g_object_set (sink, "sync", TRUE, NULL);

    gst_bin_add_many (GST_BIN (pipeline), queue, sink, NULL);
    gst_element_link_many (queue, sink, NULL);

    sinkpad = gst_element_get_static_pad (queue, "sink");
    gst_pad_link (teepad, sinkpad);
    gst_object_unref (sinkpad);

    g_timeout_add_seconds (3, tick_cb, NULL);
    linked = TRUE;
  }

  gst_caps_unref (caps);
}

int
main (int argc, char **argv)
{
  GstBus *bus;

  if (argc != 2) {
    g_error ("Usage: %s filename", argv[0]);
    return 0;
  }

  gst_init (&argc, &argv);

  pipeline = gst_pipeline_new (NULL);
  src = gst_element_factory_make ("filesrc", NULL);
  dbin = gst_element_factory_make ("decodebin", NULL);
  conv = gst_element_factory_make ("videoconvert", NULL);
  tee = gst_element_factory_make ("tee", NULL);

  if (!pipeline || !src || !dbin || !conv || !tee) {
    g_error ("Failed to create elements");
    return -1;
  }

  g_object_set (src, "location", argv[1], NULL);

  gst_bin_add_many (GST_BIN (pipeline), src, dbin, conv, tee, NULL);
  if (!gst_element_link_many (src, dbin, NULL) ||
      !gst_element_link_many (conv, tee, NULL)) {
    g_error ("Failed to link elements");
    return -2;
  }

  g_signal_connect (dbin, "pad-added", G_CALLBACK (pad_added_cb), NULL);

  loop = g_main_loop_new (NULL, FALSE);

  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  gst_bus_add_signal_watch (bus);
  g_signal_connect (G_OBJECT (bus), "message", G_CALLBACK (message_cb), NULL);
  gst_object_unref (GST_OBJECT (bus));

  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  g_main_loop_run (loop);

  gst_element_set_state (pipeline, GST_STATE_NULL);

  g_main_loop_unref (loop);

  gst_object_unref (pipeline);

  return 0;
}
#endif



#if EXAMPLE1

#include <string.h>
#include <gst/gst.h>

static GMainLoop *loop;
static GstElement *pipeline;
static GstElement *src, *dbin, *conv, *scale, *navseek, *queue, *sink;
static GstElement *conv2, *filter;
static GstPad *dbin_srcpad;
static gboolean linked = FALSE;

static gboolean
message_cb (GstBus * bus, GstMessage * message, gpointer user_data)
{
  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR:{
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
    case GST_MESSAGE_WARNING:{
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
      g_print ("Got EOS\n");
      g_main_loop_quit (loop);
      break;
    default:
      break;
  }

  return TRUE;
}

static gint in_idle_probe = FALSE;

static GstPadProbeReturn
pad_probe_cb (GstPad * pad, GstPadProbeInfo * info, gpointer user_data)
{
  GstPad *sinkpad, *srcpad;

  if (!g_atomic_int_compare_and_exchange (&in_idle_probe, FALSE, TRUE))
    return GST_PAD_PROBE_OK;

  /* Insert or remove filter? */
  if (conv2) {
    sinkpad = gst_element_get_static_pad (conv2, "sink");
    gst_pad_unlink (dbin_srcpad, sinkpad);
    gst_object_unref (sinkpad);

    sinkpad = gst_element_get_static_pad (conv, "sink");
    srcpad = gst_element_get_static_pad (filter, "src");
    gst_pad_unlink (srcpad, sinkpad);
    gst_object_unref (srcpad);

    gst_bin_remove (GST_BIN (pipeline), filter);
    gst_bin_remove (GST_BIN (pipeline), conv2);

    gst_element_set_state (filter, GST_STATE_NULL);
    gst_element_set_state (conv2, GST_STATE_NULL);

    gst_object_unref (filter);
    gst_object_unref (conv2);
    filter = NULL;
    conv2 = NULL;

    if (gst_pad_link (dbin_srcpad, sinkpad) != GST_PAD_LINK_OK) {
      g_printerr ("Failed to link src with conv\n");
      gst_object_unref (sinkpad);
      g_main_loop_quit (loop);
    }
    gst_object_unref (sinkpad);
  } else {
    conv2 = gst_element_factory_make ("videoconvert", NULL);
    filter = gst_element_factory_make ("agingtv", NULL);

    gst_object_ref (conv2);
    gst_object_ref (filter);
    gst_bin_add_many (GST_BIN (pipeline), conv2, filter, NULL);

    gst_element_sync_state_with_parent (conv2);
    gst_element_sync_state_with_parent (filter);

    if (!gst_element_link (conv2, filter)) {
      g_printerr ("Failed to link conv2 with filter\n");
      g_main_loop_quit (loop);
    }

    sinkpad = gst_element_get_static_pad (conv, "sink");
    gst_pad_unlink (dbin_srcpad, sinkpad);
    gst_object_unref (sinkpad);

    sinkpad = gst_element_get_static_pad (conv2, "sink");
    if (gst_pad_link (dbin_srcpad, sinkpad) != GST_PAD_LINK_OK) {
      g_printerr ("Failed to link src with conv2\n");
      gst_object_unref (sinkpad);
      g_main_loop_quit (loop);
    }
    gst_object_unref (sinkpad);

    srcpad = gst_element_get_static_pad (filter, "src");
    sinkpad = gst_element_get_static_pad (conv, "sink");
    if (gst_pad_link (srcpad, sinkpad) != GST_PAD_LINK_OK) {
      g_printerr ("Failed to link filter with conv\n");
      gst_object_unref (srcpad);
      gst_object_unref (sinkpad);
      g_main_loop_quit (loop);
    }
    gst_object_unref (srcpad);
    gst_object_unref (sinkpad);
  }

  return GST_PAD_PROBE_REMOVE;
}

static gboolean
timeout_cb (gpointer user_data)
{
  in_idle_probe = FALSE;
  gst_pad_add_probe (dbin_srcpad, GST_PAD_PROBE_TYPE_IDLE, pad_probe_cb, NULL,
      NULL);

  return TRUE;
}

static void
pad_added_cb (GstElement * element, GstPad * pad, gpointer user_data)
{
  GstCaps *caps;
  GstStructure *s;
  const gchar *name;

  if (linked)
    return;

  caps = gst_pad_get_current_caps (pad);
  s = gst_caps_get_structure (caps, 0);
  name = gst_structure_get_name (s);

  if (strcmp (name, "video/x-raw") == 0) {
    GstPad *sinkpad;

    sinkpad = (GstPad *)gst_element_get_static_pad (conv, "sink");
    if (gst_pad_link (pad, sinkpad) != GST_PAD_LINK_OK) {
      g_printerr ("Failed to link dbin with conv\n");
      g_main_loop_quit (loop);
    }
    gst_object_unref (sinkpad);

    dbin_srcpad = (GstPad *)gst_object_ref (pad);

    g_timeout_add_seconds (5, timeout_cb, NULL);
    linked = TRUE;
  }

  gst_caps_unref (caps);
}

int
main (int argc, char **argv)
{
  GstBus *bus;

  gst_init (&argc, &argv);

  if (argc != 2) {
    g_error ("Usage: %s filename", argv[0]);
    return -1;
  }

  pipeline = gst_pipeline_new (NULL);
  src = gst_element_factory_make ("filesrc", NULL);
  dbin = gst_element_factory_make ("decodebin", NULL);
  conv = gst_element_factory_make ("videoconvert", NULL);
  scale = gst_element_factory_make ("videoscale", NULL);
  navseek = gst_element_factory_make ("navseek", NULL);
  queue = gst_element_factory_make ("queue", NULL);
  sink = gst_element_factory_make ("autovideosink", NULL);

  if (!pipeline || !src || !dbin || !conv || !scale || !navseek || !queue
      || !sink) {
    g_error ("Failed to create elements");
    return -2;
  }

  g_object_set (src, "location", argv[1], NULL);

  gst_bin_add_many (GST_BIN (pipeline), src, dbin, conv, scale, navseek, queue,
      sink, NULL);
  if (!gst_element_link_many (src, dbin, NULL)
      || !gst_element_link_many (conv, scale, navseek, queue, sink, NULL)) {
    g_error ("Failed to link elements");
    return -3;
  }

  g_signal_connect (dbin, "pad-added", G_CALLBACK (pad_added_cb), NULL);

  loop = g_main_loop_new (NULL, FALSE);

  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  gst_bus_add_signal_watch (bus);
  g_signal_connect (G_OBJECT (bus), "message", G_CALLBACK (message_cb), NULL);
  gst_object_unref (GST_OBJECT (bus));

  if (gst_element_set_state (pipeline,
          GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
    g_error ("Failed to go into PLAYING state");
    return -4;
  }

  g_main_loop_run (loop);

  gst_element_set_state (pipeline, GST_STATE_NULL);

  g_main_loop_unref (loop);

  if (dbin_srcpad)
    gst_object_unref (dbin_srcpad);
  gst_object_unref (pipeline);

  return 0;
}
#endif
