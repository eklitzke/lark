#include <gst/gst.h>
#include <glib.h>


static gboolean bus_call (GstBus     *bus,
		GstMessage *msg,
		gpointer    data)
{
	GMainLoop *loop = (GMainLoop *) data;
	//fprintf(stderr, "bus_call\n");

	switch (GST_MESSAGE_TYPE (msg)) {
		case GST_MESSAGE_EOS:
			g_print ("End of stream\n");
			g_main_loop_quit (loop);
			break;
		case GST_MESSAGE_WARNING: 
			{
				gchar  *debug;
				GError *error;
				gst_message_parse_warning(msg, &error, &debug);
				g_printerr ("warning: %s\n", error->message);
				g_error_free (error);
				g_free (debug);
			}
			break;
		case GST_MESSAGE_INFO: 
			{
				gchar  *debug;
				GError *error;
				gst_message_parse_info(msg, &error, &debug);
				g_printerr ("info: %s\n", error->message);
				g_error_free (error);
				g_free (debug);
			}
			break;
		case GST_MESSAGE_ERROR: 
			{
				gchar  *debug;
				GError *error;

				gst_message_parse_error (msg, &error, &debug);
				g_free (debug);

				g_printerr ("Error: %s\n", error->message);
				g_error_free (error);

				g_main_loop_quit (loop);
				break;
			}
		default:
			break;
	}

	return TRUE;
}



int main (int   argc,
		char *argv[])
{
	GMainLoop *loop;
	GstElement *play;
	GstBus *bus;

	/* init GStreamer */
	gst_init (&argc, &argv);
	loop = g_main_loop_new (NULL, FALSE);

	/* make sure we have a URI */
	if (argc != 2) {
		g_print ("Usage: %s <URI>\n", argv[0]);
		return -1;
	}

	/* set up */
	play = gst_element_factory_make ("playbin", "play");
	g_object_set (G_OBJECT (play), "uri", argv[1], NULL);

	bus = gst_pipeline_get_bus (GST_PIPELINE (play));
	gst_bus_add_watch (bus, bus_call, loop);
	gst_object_unref (bus);

	gst_element_set_state (play, GST_STATE_PLAYING);

	/* now run */
	g_main_loop_run (loop);

	/* also clean up */
	gst_element_set_state (play, GST_STATE_NULL);
	gst_object_unref (GST_OBJECT (play));

	return 0;
}

