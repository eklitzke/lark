
namespace py lark.gen
namespace cpp lark

typedef string UUID

struct File { 
	1:UUID id,
	2:string uri,
	3:i64 timeModified, // unix timestamp for when this was last modified
	4:i64 timeAdded, // unix timestamp for when this was last used
	5:string album,
	6:string artist,
	7:string genre,
	8:string title,
	9:string track,
	10:string year,
	11:i64 duration,
	12:map<string, string> customFields
}

enum TermOperator {
	not_,
	equal,
	like,
	not_equal,
	less_than,
	less_than_equal,
	greater_than,
	greater_than_equal
}

struct BinaryTerm {
	1:string field,
	2:TermOperator op,
	3:string value
}

struct FileQuery {
    1:list<BinaryTerm> binaryTerms;
}

enum PlaylistType { 
	normal = 0,
}

struct Playlist {
	1: UUID id,
	2: string name,
	3: PlaylistType playlist_type,
	4: list<File> files,
	5: i64 position
}

enum Playback {
	STOPPED,
	PLAYING,
	PAUSED
}

struct Status { 
	1: Playback playback,
	2: i32 position, // the position in the playlist
	3: i32 playlistGeneration, // whenever the playlist changes, this is incremented
	4: i32 duration,
	5: i32 elapsed
}

service LarkService {
  string ping(),
  // database
  oneway void scan(1:string filesystem_path),
  oneway void remove(1:list<UUID> fileIDs),
  list<File> listFiles(1:FileQuery query),
  // not supported yet
  //void add(1:File),

  // playback commands
  oneway void enqueueByQuery(1:FileQuery query),

  list<File> playlist(),
  oneway void setPlaylist(1:list<File> files),

  oneway void setStatus(1:Status newStatus),
  Status status()

  // playlist commands
  /*
  oneway void move(1:list<UUID> fileIDs, 2:i32 position),
  list<UUID> listPlaylists(), 
  UUID createPlaylist(1:string name),
  Playlist playlistInfo(1:UUID playlistID),
  oneway void addToPlaylist(1:UUID playlistID, 2:list<UUID> songIDs)
  oneway void removeFromPlaylist(1:UUID playlistID, 2:list<UUID> songIDs)
  oneway void removePlaylist(1:UUID playlistID)
  */
}


