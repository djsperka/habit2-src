/*
 * avf1.cpp
 *
 *  Created on: May 1, 2020
 *      Author: dan
 */


#include <gst/gst.h>
#include <glib.h>

gboolean handle_keyboard (GIOChannel * source, GIOCondition cond, gpointer data);
GstElement *f_pipeline=NULL;
GMainLoop *f_mainloop=NULL;

int main(int argc, char *argv[])
{
	GstElement *camera, *convert, *sink;

	gst_init (&argc, &argv);

	// create elements
	camera = gst_element_factory_make ("avfvideosrc", "camera");
	convert = gst_element_factory_make("videoconvert", "convert");
	sink = gst_element_factory_make ("osxvideosink", "sink");

	// pipeline
	f_pipeline = gst_pipeline_new ("test-pipeline");
	if (!f_pipeline || !camera || !convert || !sink)
	{
		g_printerr("cannot create elements\n");
	}

	gst_bin_add_many(GST_BIN(f_pipeline), camera, convert, sink, NULL);
	if (gst_element_link_many(camera, convert, sink, NULL) != TRUE)
	{
		g_printerr("cannot link elements?!?\n");
		return -1;
	}

	g_print("pipeline is ready, command [rpq]: ");


  f_mainloop = g_main_loop_new( NULL, FALSE);
  GIOChannel *channel = g_io_channel_unix_new( 0 );  // 0=stdin, may need diff't call on windows
  g_io_add_watch( channel, G_IO_IN, handle_keyboard, f_mainloop);
  g_main_loop_run(f_mainloop);

  /* Free resources */
  gst_element_set_state (f_pipeline, GST_STATE_NULL);
  gst_object_unref (f_pipeline);
  return 0;
}

/* Process keyboard input */
gboolean handle_keyboard (GIOChannel * source, GIOCondition c, gpointer data)
{
  gchar *str = NULL;
  GstStateChangeReturn ret;

  if (g_io_channel_read_line (source, &str, NULL, NULL,
          NULL) != G_IO_STATUS_NORMAL) {
    return TRUE;
  }

  switch (g_ascii_tolower (str[0])) {
  case 'r':
  {
	  ret = gst_element_set_state(f_pipeline, GST_STATE_READY);
	  g_print("gst_element_set_state(pipeline, GST_STATE_READY) returned %s\n", gst_element_state_change_return_get_name(ret));
	  break;
  }
  case 'p':
  {
	  ret = gst_element_set_state(f_pipeline, GST_STATE_PLAYING);
	  g_print("gst_element_set_state(pipeline, GST_STATE_PLAYING) returned %s\n", gst_element_state_change_return_get_name(ret));
	  break;
  }
  case 'q':
      g_main_loop_quit (f_mainloop);
      break;
  default:
      break;
  }

  g_free (str);

  return TRUE;
}



