
namespace py lark.gen
namespace cpp lark

typedef string UUID 

struct File { 
	1:UUID id,
	2:string fileSystemPath,
	3:i64 timeModified, // unix timestamp for when this was last modified
	4:i64 timeAdded, // unix timestamp for when this was last used
	5:string album,
	6:string artist,
	7:string genre,
	8:string title,
	9:string track,
	10:string year,
	11:i64 duration
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
	4: list<File> files
}

enum PlayState {
	STOPPED,
	PLAYING,
	PAUSED
}

service LarkService {
  string ping(),
  // database
  oneway void scan(1:string filesystem_path),
  oneway void remove(1:list<UUID> file_ids),

  // play commands
  oneway void setState(1:PlayState newState),
  oneway void playByQuery(1:FileQuery query),
  oneway void playURL(1:string URL), 

  // playlist commands
  oneway void move(1:list<UUID> fileIDs, 2:i32 position),
  list<File> listFiles(1:FileQuery query),
  list<UUID> listPlaylists(), 
  UUID createPlaylist(1:string name),
  Playlist playlistInfo(1:UUID playlistID),
  oneway void addToPlaylist(1:UUID playlistID, 2:list<UUID> songIDs)
  oneway void removeFromPlaylist(1:UUID playlistID, 2:list<UUID> songIDs)
  oneway void removePlaylist(1:UUID playlistID)
}


