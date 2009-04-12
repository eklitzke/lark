#include "sqlite3_store.h"

#include <iostream>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem/operations.hpp>
#include <tag.h>
#include <fileref.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/lexical_cast.hpp>
#include <sqlite3.h>
#include <server/TSimpleServer.h>
#include <server/TThreadPoolServer.h>
#include <server/TThreadedServer.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>

#include "gen/LarkService.h"
#include "player.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;


#ifndef DAEMON_H
#define DAEMON_H
using namespace std;
using namespace boost;
using namespace lark;

class LarkServiceHandler : virtual public LarkServiceIf {
 private:
  shared_ptr<SQLite3Store> dataStore_;
  shared_ptr<Player> player_;
 public:
  void setPlayer(shared_ptr<Player> player) {
	  player_ = player;
  } 
  void setDataStore(shared_ptr<SQLite3Store> dataStore) {
	  dataStore_ = dataStore;
	  shared_ptr<Player> player(new Player(dataStore));
	  setPlayer(player) ;
  };

  LarkServiceHandler(shared_ptr<SQLite3Store> dataStore) { 
	  setDataStore(dataStore);
  };

  void ping(std::string & _result) {
    // Your implementation goes here
    printf("ping\n");
	_result = "pong";
  }

  void scan(const std::string& filesystem_path) {
    // Your implementation goes here
    printf("scan\n");
	dataStore_->file_store()->scan(filesystem_path); 
  }

  void remove(const std::vector<UUID> & file_ids) {
    // Your implementation goes here
    printf("remove\n");
  }

  void setState(const PlayState newState) {
    // Your implementation goes here
    printf("setState\n");
  }

  void play(const UUID& file_id) {
    // Your implementation goes here
    printf("play\n");
  }

  void playURL(const string & url) {
	  cout << "play url:" << url << endl;
	  player_->playURL(url);
  }

  void playByQuery(const FileQuery & query)  {
	  player_->playByQuery(query);
  }

  void move(const std::vector<UUID> & fileIDs, const int32_t position) {
    // Your implementation goes here
    printf("move\n");
  }

  void listFiles(vector<lark::File> & _return, const FileQuery & query) {
    // Your implementation goes here
    printf("listFiles\n");
	_return = *(dataStore_->file_store()->list(query));	
  }

  void listPlaylists(std::vector<UUID> & _return) {
    // Your implementation goes here
    printf("listPlaylists\n");
  }

  void createPlaylist(UUID& _return, const std::string& name) {
    // Your implementation goes here
    printf("createPlaylist\n");
	_return = *dataStore_->playlist_store()->create(name);
  }

  void playlistInfo(Playlist& _return, const UUID& playlistID) {
    // Your implementation goes here
    printf("playlistInfo\n");
  }

  void addToPlaylist(const UUID& playlistID, const std::vector<UUID> & songIDs) {
    // Your implementation goes here
    printf("addToPlaylist\n");
	dataStore_->playlist_store()->add(playlistID, songIDs);
  }

  void removeFromPlaylist(const UUID& playlistID, const std::vector<UUID> & songIDs) {
    // Your implementation goes here
    printf("removeFromPlaylist\n");
  }
  void removePlaylist(const UUID& playlistID) {
    // Your implementation goes here
    printf("removePlaylist\n");
	dataStore_->playlist_store()->remove(playlistID);
  }
};

#endif
