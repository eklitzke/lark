
namespace py lark.gen
namespace cpp lark

typedef string UUID 

struct File { 
  UUID id,
  string fileSystemPath,
  i64 timeModified, // unix timestamp for when this was last modified
  i64 timeAdded, // unix timestamp for when this was last used
  string album,
  string artist,
  string genre,
  string title,
  string track,
  string year,
  i64 duration,
}

enum PlaylistType { 
  normal = 0,
}

struct Playlist {
  UUID id,
  string name,
  PlaylistType playlist_type,
  list<File> songs;
}

enum PlayState {
	STOPPED,
	PLAYING,
	PAUSED
}

service LarkService {
  void ping(),
  // database
  oneway void scan(string filesystem_path),
  oneway void remove(list<UUID> song_ids),

  // play commands
  oneway void setState(PlayState newState),
  oneway void play(UUID fileID),
  oneway void playURL(string URL), 

  // playlist commands
  oneway void move(list<UUID> fileIDs, i32 position),
  list<File> listFiles(),
  list<Playlist> listPlaylists()
}


