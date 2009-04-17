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
	  player_->start();
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
	_result = "pong";
  }

  void scan(const std::string& filesystem_path) {
	dataStore_->file_store()->scan(filesystem_path); 
  }

  void remove(const std::vector<UUID> & file_ids) {
  }

  virtual void setStatus(const Status & newState) {
	  player_->setStatus(newState);
  }

  virtual void status(Status & _return) {
	_return = *player_->status();
  }

  virtual void playlist(Files & _return) { 
	   _return = *player_->playlist();
  }

  void enqueueByQuery(const FileQuery & query)  {
	  player_->enqueueByQuery(query);
  }

  void listFiles(vector<lark::File> & _return, const FileQuery & query) {
	_return = *(dataStore_->file_store()->list(query));	
  }

};

#endif
