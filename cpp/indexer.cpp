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

using namespace std;
using namespace boost;

namespace fs = boost::filesystem;

class SQLite3Error: public boost::exception {  
	public:
		SQLite3Error(int result_code) {
			this->_result_code = result_code;
		};
		int result_code() { 
			return _result_code;
		}
	private:
		int _result_code;
};

string new_uuid();
void print_usage(const char *name);

void print_usage(const char *name) {
	cout << name << ":";
}

static const string ARTIST = "artist";
static const string ALBUM = "album";
static const string TITLE = "album";
static const string GENRE = "genre";
static const string TRACK = "track";
static const string YEAR = "year";

class Indexer {
	public:
		Indexer(const string & database_path);
		virtual ~Indexer();
		virtual void scan(const string &path);
	protected:
		virtual void insert(const string & songID, const string & path);
		virtual void init_db();
		virtual void insertField(const string & songID, const string & field, const string & value);
		virtual int check_sqlite3_result(int result_code);
		virtual bool exists(const string & path); 
	private:
		sqlite3 *db;
	
};

Indexer::~Indexer() {
	if (this->db != NULL) {
		cerr << "closing db" << endl;
		int result_code = sqlite3_close(this->db);
		this->db = NULL;
	}
}

Indexer::Indexer(const string & database_path) {
	db = NULL;
	this->check_sqlite3_result(sqlite3_open(database_path.c_str(), &this->db));
	this->init_db();
};

bool Indexer::exists(const string & path) { 
	
	return true;
}
void Indexer::init_db() { 
	this->check_sqlite3_result(sqlite3_exec(this->db, "create table if not exists "
		" song (id text primary key, path text default '') ", NULL, NULL, NULL));
	this->check_sqlite3_result(sqlite3_exec(this->db, "create table if not exists "
		" song_field (id integer unsigned primary key, song_id text, field text, value text)", NULL, NULL, NULL));
}

void Indexer::scan(const string & a_path) {
	if (!fs::exists(a_path)) 
		return;
	if (fs::is_directory(a_path)) {
		fs::directory_iterator end; // default construction yields past-the-end
		for (fs::directory_iterator curr(a_path); curr != end; ++curr) {
			fs::path p = curr->path();
			string s = p.string();
			this->scan(s);
		}
	} else if (fs::is_regular_file(a_path)) {
		TagLib::FileRef file_ref(a_path.c_str(), false);
		if (!file_ref.isNull() && file_ref.tag()) {
			cout << endl;
			cout << a_path << endl;
			TagLib::Tag *t = file_ref.tag();
			string artist = t->artist().to8Bit(true);
			string album = t->album().to8Bit(true);
			string title = t->title().to8Bit(true);
			string genre = t->genre().to8Bit(true);
			string year = t->year() > 0 ? lexical_cast<string>(t->year()) : "";
			string track = t->track() > 0 ? lexical_cast<string>(t->track()) : "";
			string songID = new_uuid();
			this->insert(songID, a_path);
			this->insertField(songID, ARTIST, artist);
			this->insertField(songID, ALBUM, album);
			this->insertField(songID, YEAR, year);
			this->insertField(songID, TITLE, title);
			this->insertField(songID, GENRE, genre);
			this->insertField(songID, TRACK, track);
		}
	}
}

string new_uuid() {
	uuid_t *a_uuid;
	size_t sz = 0;
	uuid_create(&a_uuid);
	uuid_make(a_uuid, UUID_MAKE_V1);
	void *uuid_out = NULL;
	uuid_export(a_uuid, UUID_FMT_STR, &uuid_out, &sz);
	string result((const char *)uuid_out, sz);
	free(uuid_out);
	uuid_destroy(a_uuid);
	return result;
}


void Indexer::insert(const string & songID, const string & path) {
	cout << "insert" << songID << endl;
	sqlite3_stmt *insert_statement = NULL;
	string query = "insert into song (id, path) values (?, ?)";
	this->check_sqlite3_result(sqlite3_prepare(this->db, query.c_str(), -1, &insert_statement, NULL));
	this->check_sqlite3_result(sqlite3_bind_text(insert_statement, 1, songID.c_str(), songID.size(), SQLITE_STATIC));
	this->check_sqlite3_result(sqlite3_bind_text(insert_statement, 2, path.c_str(), path.size(), SQLITE_STATIC));
	this->check_sqlite3_result(sqlite3_step(insert_statement));
}

int Indexer::check_sqlite3_result(int result_code) {
	switch (result_code) {
		case SQLITE_OK:
		case SQLITE_DONE:
		case SQLITE_ROW:
			break;
		default:
			throw SQLite3Error(result_code);
	}
	return result_code;
}

void Indexer::insertField(const string & songID, const string & field, const string & value) {
	if (value.size() == 0)
		return;
	cout << "(" << songID << ", " << field << ", " << value << ")" << endl;
	sqlite3_stmt *insert_statement = NULL;
	string query = "insert into song_field (song_id, field, value) values (?, ?, ?)";
	this->check_sqlite3_result(sqlite3_prepare(this->db, query.c_str(), -1, &insert_statement, NULL));
	this->check_sqlite3_result(sqlite3_bind_text(insert_statement, 1, songID.c_str(), songID.size(), SQLITE_STATIC));
	this->check_sqlite3_result(sqlite3_bind_text(insert_statement, 2, field.c_str(), field.size(), SQLITE_STATIC));
	this->check_sqlite3_result(sqlite3_bind_text(insert_statement, 3, value.c_str(), value.size(), SQLITE_STATIC));
	this->check_sqlite3_result(sqlite3_step(insert_statement));

}

int main(int argc, char *argv[]) {
    int next_option;
    const char* const short_options = "hp:c:";
    const struct option long_options[] = {
      { "help",   0, NULL, 'h' },
      { NULL,     0, NULL, 'o' },
    };
	string database_name = "songs.db";

    while (0 < (next_option = getopt_long(argc, argv, short_options, long_options, NULL))) {
      switch (next_option) {
		  default:
			  print_usage(argv[0]);
			  return 1;
	  }
	}

	vector<string> paths;
	for (int i = optind; i < argc; i++) {
		paths.push_back(argv[i]);
	}

	Indexer indexer(database_name);

	vector<string>::const_iterator cii;
	for (cii = paths.begin(); cii != paths.end(); ++cii) {
		string s(*cii);
		try {
			indexer.scan(s);
		} catch (SQLite3Error & sqlite_error) {
			cout << "sqlite3 error:" << sqlite_error.result_code() << endl;
			return 1;
		}
	}
	cout << endl;

	return 0;
}




