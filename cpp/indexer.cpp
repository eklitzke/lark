#include "indexer.h"
namespace lark {

static const string ARTIST = "artist";
static const string ALBUM = "album";
static const string TITLE = "album";
static const string GENRE = "genre";
static const string TRACK = "track";
static const string YEAR = "year";

void print_usage(const char *name);

void print_usage(const char *name) {
	cout << name << ":" << endl;
	cout << "\t-h\t\tThis message." << endl;
}

Indexer::~Indexer() {
	if (this->db != NULL) {
		//int result_code = 
		sqlite3_close(this->db);
		this->db = NULL;
	}
}

Indexer::Indexer(const string & database_path) {
	db = NULL;
	this->check_sqlite3_result(sqlite3_open(database_path.c_str(), &this->db));
	this->init_db();
};

void Indexer::listFiles(vector<File> & _return) {
	sqlite3_stmt *select_statement = NULL;
	string query = "select s.id, s.path, sf.field, sf.value from song s, song_field sf where sf.song_id = s.id order by s.id asc";
	this->check_sqlite3_result(sqlite3_prepare(this->db, query.c_str(), -1, &select_statement, NULL));
	int num = 0;

	std::string lastID = "";
	lark::File *a_file = NULL;
	while (true) {
		int result_code = this->check_sqlite3_result(sqlite3_step(select_statement));
		if (result_code == SQLITE_ROW) {
			std::string id = (const char *)sqlite3_column_text(select_statement, 0);
			std::string filePath = (const char *)sqlite3_column_text(select_statement, 1);
			std::string fieldName = (const char *)sqlite3_column_text(select_statement, 2);
			std::string fieldValue = (const char *)sqlite3_column_text(select_statement, 3);

			if (id != lastID) {
				if (a_file != NULL) {
					_return.push_back(*a_file);
					delete a_file; 
					a_file = NULL;
				}
				a_file = new lark::File();
				a_file->id = id;
				a_file->fileSystemPath = filePath;
				lastID = id;
			}
			if (fieldName == ARTIST) 
				a_file->artist = fieldValue;
			else if (fieldName == ALBUM) 
				a_file->album = fieldValue;
			else if (fieldName == TITLE) 
				a_file->title = fieldValue;
			else if (fieldName == GENRE) 
				a_file->genre = fieldValue;
			else if (fieldName == YEAR) 
				a_file->year = fieldValue;
			else if (fieldName == TRACK) 
				a_file->track = fieldValue;
		} else if (result_code == SQLITE_DONE) {
			this->check_sqlite3_result(sqlite3_finalize(select_statement));
			break;
		}
	}
	if (a_file != NULL) {
		_return.push_back(*a_file);
		delete a_file; 
	}
}

bool Indexer::path_exists(const string & path) { 
	sqlite3_stmt *select_statement = NULL;
	string query = "select count(*) from song where path = ?";
	this->check_sqlite3_result(sqlite3_prepare(this->db, query.c_str(), -1, &select_statement, NULL));
	this->check_sqlite3_result(sqlite3_bind_text(select_statement, 1, path.c_str(), path.size(), SQLITE_STATIC));
	int num = 0;
	while (true) {
		int result_code = this->check_sqlite3_result(sqlite3_step(select_statement));
		if (result_code == SQLITE_ROW) {
			num = sqlite3_column_int(select_statement, 0);
		} else if (result_code == SQLITE_DONE) {
			this->check_sqlite3_result(sqlite3_finalize(select_statement));
			break;
		}
	}
	return (num > 0);
}

void Indexer::init_db() { 
	this->check_sqlite3_result(sqlite3_exec(this->db, "create table if not exists "
		" song (id text primary key, path text default '') ", NULL, NULL, NULL));
	this->check_sqlite3_result(sqlite3_exec(this->db, "create table if not exists "
		" song_field (id integer unsigned primary key, song_id text, field text, value text)", NULL, NULL, NULL));
	this->check_sqlite3_result(sqlite3_exec(this->db, "create index if not exists "
		" song_path_idx ON song(path) ", NULL, NULL, NULL));
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
		if (this->path_exists(a_path))
			return;
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
	this->check_sqlite3_result(sqlite3_finalize(insert_statement));
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
	this->check_sqlite3_result(sqlite3_finalize(insert_statement));
}

}

