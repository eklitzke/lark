

typedef string UUID 

struct Song { 
  UUID id,
  string artist,
  string album,
  string title,
  string year,
  string track,
  string genre,
  string comment,
  string filesystem_path,
  i64 duration

}

enum PlaylistType { 
  normal = 0,
}

struct Playlist {
  UUID id,
  string name,
  PlaylistType playlist_type,
  list<Song> songs;
}

enum PlayState {
	STOPPED,
	PLAYING,
	PAUSED
}

service SongService {
  void ping(),
  // database
  void scan(string filesystem_path),
  void remove(list<UUID> song_ids),

  // play commands
  void setState(PlayState newState),
  void play(UUID song_id),

  // playlist commands
  void move(list<UUID> songs, i32 position),

  list<Song> listSongs(),
  list<Playlist> listPlaylists()

  
}


