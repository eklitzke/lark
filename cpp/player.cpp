#include "player.h"

namespace lark {

	void getField(const File & aFile, const string & field, string & result) {
		if (field == ARTIST)
			result = aFile.artist;
		else if (field == ALBUM) 
			result = aFile.album;
		else if (field == ID)
			result = aFile.id;
		else if (field == TRACK) 
			result = aFile.track;
		else if (field == GENRE)
			result = aFile.genre;
		else if (field == TITLE)
			result = aFile.title;
		else
			result = "";
		//cerr << "field: " << field << "result: " << result << endl;
	}

	class FileComparator {
		public:
			FileComparator(shared_ptr<FileSort> sort) : sort_(sort) {
			}
			virtual ~FileComparator() { } ;

			bool operator () (const File & a, const File & b) {
				for (unsigned int i = 0; i < sort_->sortFields.size(); i++) {
					SortField sortField = sort_->sortFields[i];
					if (sortField.field.size() == 0)
						continue;
					string left, right;
					getField(a, sortField.field, left);
					getField(b, sortField.field, right);
					//cerr << "left: " << left << "right: " << right << endl;
					// FIXME: Replace this with a UTF8 natural language compare
					int cmp = left.compare(right);
					if (cmp == 0)
						continue;
					return cmp < 0;
					//if (cmp < 0)
					//	return true;
					//return (sortField.order == ASCENDING && cmp < 0);
				}
				return false;
			};

		private:
			shared_ptr<FileSort> sort_;
	};
	
	gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data) {
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
		shared_ptr<FileSort> defaultSort(new FileSort);
		SortField artistField, albumField, trackField, titleField;
		artistField.field = ARTIST;
		defaultSort->sortFields.push_back(artistField);
		albumField.field = ALBUM;
		defaultSort->sortFields.push_back(albumField);
		trackField.field = TRACK;
		defaultSort->sortFields.push_back(trackField);
		titleField.field = TITLE;
		defaultSort->sortFields.push_back(titleField);
		shared_ptr<FileQuery> defaultQuery(new FileQuery);
		updateFilterAndSort(defaultQuery, defaultSort);
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
	}

	bool Player::updateFilterAndSort(shared_ptr<FileQuery> fileFilter, shared_ptr<FileSort> fileSort) {
		if (*filter_ == *fileFilter && *fileSort == *sort_) {
			cerr << "The file filter and sort has not changed, skipping update" << endl;
			return false;
		}
				
		shared_ptr<Files> result;
		if (*fileFilter != *filter_ || playlist_->size() == 0) {
			cerr << "The file filter has changed, reloading files" << endl;
			result = dataStore_->file_store()->list(*fileFilter);
		} else {
			cerr << "Reusing old file list" << endl;  
			result.reset(new Files(*playlist_));
		}
		cout << "found " << result->size() << " files" << endl;
		FileComparator fileComparator(fileSort);
		sort(result->begin(), result->end(), fileComparator);
		playlist_ = result;
		filter_ = fileFilter;
		sort_ = fileSort;
		cerr << "done reloading files" << endl;
		return true;
	}

	void Player::setStatus(const Status& newStatus) {
		Status currStatus = *status();

		shared_ptr<FileQuery> file_filter(new FileQuery);
		*file_filter = newStatus.filter;

		shared_ptr<FileSort> file_sort(new FileSort);
		*file_sort = newStatus.sort;

		updateFilterAndSort(file_filter, file_sort);

		switch (currStatus.playback) {
			case PLAYING:
				switch (newStatus.playback) {
					case PLAYING:
						if (currStatus.position != newStatus.position)
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
						if (newStatus.position == currStatus.position)
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
		while (1) {
			cerr << "playAt: " << newPosition << endl;
			if (newPosition >= playlist_->size())
				return;
			playlistPosition_ = newPosition;
			File file = (*playlist_)[newPosition];
			if (file.uri.size() > 0) {
				playURI(file.uri);
				return;
			}
			newPosition++;
		}
	}

	shared_ptr<Status> Player::status() {
		shared_ptr<Status> result(new Status);
		GstState pending;
		GstState curr;
		gst_element_get_state(playElement, &pending, &curr, 0);
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
		result->sort = *sort_;
		result->filter = *filter_;
		result->position = playlistPosition_;
		return result;
	}
}


