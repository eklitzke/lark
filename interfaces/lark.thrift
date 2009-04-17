
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

enum Order { 
	ASCENDING = 0,
	DESCENDING = 1
}

struct SortField { 
	1: string field,
	2: Order order
}

struct FileSort { 
	1: list<SortField> sortFields
}

enum Playback {
	STOPPED = 0,
	PLAYING,
	PAUSED
}

enum Mode { 
	LIBRARY = 0
}

struct Status { 
	1: Playback playback,
	2: i32 position, // the position in the playlist
	4: i32 duration,
	5: i32 elapsed,
	6: Mode playlistMode,
	7: FileSort sort,
	8: FileQuery filter
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
  oneway void setStatus(1:Status newStatus),
  Status status()
}


