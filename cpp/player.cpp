#include "player.h"

namespace lark {
	gboolean bus_call (GstBus *bus, GstMessage *msg, gpointer data) {
		Player *player = (Player *)data;
		return player->busEvent(bus, msg);
	}

	gboolean Player::busEvent(GstBus *bus, GstMessage *msg) {
		switch (GST_MESSAGE_TYPE (msg)) {
			case GST_MESSAGE_EOS:
				g_print ("End of stream\n");
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
					break;
				}
			default:
				break;
		}

		return TRUE;
	};

	Player::Player(shared_ptr<SQLite3Store> dataStore) { 
		dataStore_ = dataStore;
		eventThread_.reset(new thread (&Player::eventLoop, this));
	};

	void Player::eventLoop() {
		cout << "event loop" << endl;
		GstBus *bus;
		char *argv[1] = {"larkd"};
		//argv[0] = "larkd";
		int argc = 0;
		gst_init(&argc, (char ***)&argv);
		loop = g_main_loop_new(NULL, FALSE);
		playElement = gst_element_factory_make("playbin", "play");
		bus = gst_pipeline_get_bus(GST_PIPELINE(playElement));
		gst_bus_add_watch(bus, bus_call, this);
		/* now run */
		g_main_loop_run (loop);
		/* also clean up */
	};

	Player::~Player() {
		gst_element_set_state (playElement, GST_STATE_NULL);
		gst_object_unref(GST_OBJECT (playElement));
	};

	void Player::playURL(const string & uri) {
		gst_element_set_state (playElement, GST_STATE_NULL);
		g_object_set(G_OBJECT(playElement), "uri", uri.c_str(), NULL);
		gst_element_set_state(playElement, GST_STATE_PLAYING);
	};
	void Player::playByQuery(const FileQuery & query) {

	}
}


