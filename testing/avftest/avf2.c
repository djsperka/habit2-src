/*
 * avf1.cpp
 *
 *  Created on: May 1, 2020
 *      Author: dan
 */





/*
 *
 *
 *   +----------------+   +----------------+   +----------------+
 *   |  videotestsrc  |---|  videoconvert  |---|  osxvideosink  |
 *   +----------------+   +----------------+   +----------------+
 *
 *   Initially the above is created and set PLAYING
 *   command 'c' creates camera and adds to pipeline.
 *           'r' sets camera element to GST_STATE_READY (triggers check)
 *           'R' creates (supposed to) videoconvert and osxvideosink, sets to READY
 *           'p' sets pipeline to PLAYING
 *           'z' sets pipeline to PAUSED
 *           'd' dumps pipeline diagram to file (set GST_DEBUG_DUMP_DOT_DIR to destination folder)
 *
 *   +----------------+   +----------------+   +----------------+
 *   |  avfvideosrc   |---|  videoconvert  |---|  osxvideosink  |
 *   +----------------+   +----------------+   +----------------+
 *
 *
 */

#include <gst/gst.h>
#include <glib.h>

//#include <libavdevice/avdevice.h>

gboolean handle_keyboard (GIOChannel * source, GIOCondition cond, gpointer data);
GstElement *f_pipeline=NULL;
GMainLoop *f_mainloop=NULL;
GstElement *f_camera=NULL;
GstElement *f_cameraConvert=NULL;
GstElement *f_cameraSink=NULL;

int main(int argc, char *argv[])
{

// pkg-config doesn't seem to like the combo of gstreamer-1.0 and libavdevice?
// getting err gst/gst.h not found, etc.
//
//	AVOutputFormat *d = av_output_video_device_next(NULL);
//	g_print("Getting video output devices\n");
//	while (d)
//	{
//		g_print("video device: %s  %s\n", d->name, d->long_name);
//		d = av_output_video_device_next(d);
//	}
//	g_print("Getting video output devices done\n");
//	d = av_output_audio_device_next(NULL);
//	g_print("Getting audio output devices\n");
//	while (d)
//	{
//		g_print("audio device: %s  %s\n", d->name, d->long_name);
//		d = av_output_audio_device_next(d);
//	}
//	g_print("Getting audio output devices done\n");
//
//

	GstElement *src, *convert, *sink;

	gst_init (&argc, &argv);

	// create elements
	src = gst_element_factory_make ("videotestsrc", "video");
	convert = gst_element_factory_make("videoconvert", "convert");
	sink = gst_element_factory_make ("osxvideosink", "sink");

	// pipeline
	f_pipeline = gst_pipeline_new ("test-pipeline");
	if (!f_pipeline || !src || !convert || !sink)
	{
		g_printerr("cannot create elements\n");
	}

	gst_bin_add_many(GST_BIN(f_pipeline), src, convert, sink, NULL);
	if (gst_element_link_many(src, convert, sink, NULL) != TRUE)
	{
		g_printerr("cannot link elements?!?\n");
		return -1;
	}
	gst_element_set_state(f_pipeline, GST_STATE_PLAYING);

	g_print("pipeline is playing, command [crpq]: ");


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
  GError *gerror;

  if (g_io_channel_read_line (source, &str, NULL, NULL,
          NULL) != G_IO_STATUS_NORMAL) {
    return TRUE;
  }

  switch (g_ascii_tolower (str[0])) {
  case 'c':
  {
	  f_camera = gst_element_factory_make ("avfvideosrc", "camera");
	  gst_bin_add(GST_BIN(f_pipeline), f_camera);
	  break;
  }
  case 'z':
  {
	  gst_element_set_state(f_pipeline, GST_STATE_PAUSED);
	  break;
  }
  case 'r':
  {
	  ret = gst_element_set_state(f_camera, GST_STATE_READY);
	  g_print("gst_element_set_state(pipeline, GST_STATE_READY) returned %s\n", gst_element_state_change_return_get_name(ret));
	  break;
  }
  case 's':
  {
		f_cameraConvert = gst_element_factory_make("videoconvert", "convertcam");
		f_cameraSink = gst_element_factory_make ("osxvideosink", "sinkcamera");

		gst_bin_add_many(GST_BIN(f_pipeline), f_cameraConvert, f_cameraSink, NULL);
		if (gst_element_link_many(f_camera, f_cameraConvert, f_cameraSink, NULL) != TRUE)
		{
			g_printerr("cannot link camera elements?!?\n");
			return -1;
		}
		gst_element_set_state(f_cameraConvert, GST_STATE_READY);
		gst_element_set_state(f_cameraSink, GST_STATE_READY);
		break;
  }
  case 'p':
  {
	  ret = gst_element_set_state(f_pipeline, GST_STATE_PLAYING);
	  g_print("gst_element_set_state(pipeline, GST_STATE_PLAYING) returned %s\n", gst_element_state_change_return_get_name(ret));
	  break;
  }
  case 'd':
  {
		GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(f_pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "avf2");
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



