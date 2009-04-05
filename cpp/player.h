#include <iostream>
#include <getopt.h>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/thread.hpp>

#include "gen/lark_types.h"

#ifndef PLAYER_H
#define PLAYER_H

using namespace std;
using namespace boost;

namespace lark {
	class Player { 
		public:
			Player();
			virtual ~Player();
			virtual void playURL(const string & url);
		private:
			shared_ptr<lark::File> *currentSong;
	};
}

#endif
