#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>

#include <ossp/uuid.h>
#include <iostream>
#include <getopt.h>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem/operations.hpp>
#include <tag.h>
#include <fileref.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/lexical_cast.hpp>
#include <sqlite3.h>

#include "gen/LarkService.h"

#include <server/TSimpleServer.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;


#ifndef DAEMON_H
#define DAEMON_H
using namespace std;
using namespace boost;

class LarkServiceHandler : virtual public LarkServiceIf {
 public:
  LarkServiceHandler() {
    // Your initialization goes here
  }

  void ping() {
    // Your implementation goes here
    printf("ping\n");
  }

  void scan(const std::string& filesystem_path) {
    // Your implementation goes here
    printf("scan\n");
  }

  void remove(const std::vector<UUID> & song_ids) {
    // Your implementation goes here
    printf("remove\n");
  }

  void setState(const PlayState newState) {
    // Your implementation goes here
    printf("setState\n");
  }

  void play(const UUID& song_id) {
    // Your implementation goes here
    printf("play\n");
  }

  void move(const std::vector<UUID> & songs, const int32_t position) {
    // Your implementation goes here
    printf("move\n");
  }

  void listSongs(std::vector<Song> & _return) {
    // Your implementation goes here
    printf("listSongs\n");
  }

  void listPlaylists(std::vector<Playlist> & _return) {
    // Your implementation goes here
    printf("listPlaylists\n");
  }

};

#endif