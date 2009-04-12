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
			Player(shared_ptr<SQLite3Store> dataStore);
			virtual ~Player();
			virtual void playURL(const string & url);
			virtual void playByQuery(const FileQuery & query);
			virtual void eventLoop();
			virtual gboolean busEvent(GstBus *bus, GstMessage *msg);
		private:
			GstElement *playElement;
			GMainLoop *loop;
			shared_ptr<thread> eventThread_;
			shared_ptr<SQLite3Store> dataStore_;
			shared_ptr<string> currentPlaylistID_;
	};

}

#endif
