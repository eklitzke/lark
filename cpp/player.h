#include <iostream>
#include <getopt.h>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/thread.hpp>
#include <gst/gst.h>
#include <glib.h>

#include "gen/lark_types.h"
#include "sqlite3_store.h"

#ifndef PLAYER_H
#define PLAYER_H

using namespace std;
using namespace boost;

namespace lark {
	class Player { 
		public:
			Player(shared_ptr<SQLite3Store> dataStore) : dataStore_(dataStore), playlistPosition_(-1), playlistGeneration_(0),
			playlist_(new Files) { }
			virtual ~Player();
			virtual void start();
			/* Play all of the items in a file query. */
			virtual void enqueueByQuery(const FileQuery & query);
			virtual void eventLoop();
			virtual gboolean busEvent(GstBus *bus, GstMessage *msg);
			virtual shared_ptr<Files> playlist() { 
				return playlist_; 
			}
			virtual void setPlaylist(const Files& files) { 
				*playlist_ = files; 
				playlistGeneration_++; 
			}
			virtual shared_ptr<Status> status();
			virtual void setStatus(const Status&);
		private:
			virtual void stop();
			virtual void resume();
			virtual void pause();
			virtual void playAt(unsigned int);
			virtual void playURI(string uri);
			GstElement *playElement;
			GMainLoop *loop;
			shared_ptr<thread> eventThread_;
			shared_ptr<SQLite3Store> dataStore_;
			shared_ptr<UUID> currentPlaylistID_;
			int playlistPosition_;
			int playlistGeneration_;
			shared_ptr<Files> playlist_;
	};

}

#endif
