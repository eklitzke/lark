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
				playAt(playlistPosition_ + 1);
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

	void Player::start() {
		if (eventThread_.use_count() == 0)
			eventThread_.reset(new thread (&Player::eventLoop, this));
	}

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

	void Player::playURI(string uri) {
		cout << "playing uri: " << uri << endl;
		gst_element_set_state (playElement, GST_STATE_NULL);
		g_object_set(G_OBJECT(playElement), "uri", uri.c_str(), NULL);
		gst_element_set_state(playElement, GST_STATE_PLAYING);
	};
	
	void Player::enqueueByQuery(const FileQuery & query) {
		shared_ptr<Files> files = dataStore_->file_store()->list(query);
		for (unsigned int i = 0; i < files->size(); i++) {
			File file((*files)[i]);
			playlist_->push_back(file);	
		}
		playlistGeneration_++;
	}

	void Player::setStatus(const Status& newStatus) {
		shared_ptr<Status> currStatus(status());
		switch (currStatus->playback) {
			case PLAYING:
				switch (newStatus.playback) {
					case PLAYING:
						if (currStatus->position != newStatus.position)
							playAt(newStatus.position);
						break;
					case PAUSED:
						pause();
						break;
					case STOPPED:
						stop();
						break;
				}
				break;
			case STOPPED:
				switch (newStatus.playback) {
					case PLAYING:
						playAt(newStatus.position);
						break;
					case PAUSED:
						break;
					case STOPPED:
						break;
				}
				break;
			case PAUSED:
				switch (newStatus.playback) {
					case PLAYING:
						if (newStatus.position == currStatus->position)
							resume();
						else
							playAt(newStatus.position);
						break;
					case PAUSED:
						break;
					case STOPPED:
						stop();
						break;
				}
				break;
		}
	}

	void Player::pause() {
		gst_element_set_state (playElement, GST_STATE_PAUSED);
	}
	void Player::stop() {
		gst_element_set_state (playElement, GST_STATE_NULL);
	}

	void Player::resume() {
		gst_element_set_state (playElement, GST_STATE_PLAYING);
	}

	void Player::playAt(unsigned int newPosition) {
		
		if (playlist_->size() < newPosition)  {
			return;
		}
		playlistPosition_ = newPosition;
		File file = (*playlist_)[playlistPosition_];
		if (file.uri.size() > 0) {
			playURI(file.uri);
		}
	}

	shared_ptr<Status> Player::status() {
		shared_ptr<Status> result(new Status);
		GstState pending;
		GstState curr;
		GstStateChangeReturn st = gst_element_get_state(playElement, &pending, &curr, 0);
		switch (curr) {
			case GST_STATE_PLAYING:
				result->playback = PLAYING;
				break;
			case GST_STATE_PAUSED:
				result->playback = PAUSED;
				break;
			default:
				result->playback = STOPPED;
				break;
		}
		result->position = playlistPosition_;
		result->playlistGeneration = playlistGeneration_;
		return result;
	}
}


