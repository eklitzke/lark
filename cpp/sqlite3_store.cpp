// This needs to be first to overcome a conflict on MacOSX between unistd and ossp/uuid.h
#include <ossp/uuid.h>

#include "sqlite3_store.h"

namespace lark {
	static const string PATH = "path";
	static const string ARTIST = "artist";
	static const string ALBUM = "album";
	static const string TITLE = "title";
	static const string GENRE = "genre";
	static const string TRACK = "track";
	static const string YEAR = "year";

	shared_ptr<Files> FileStore::list(const FileQuery & fileQuery) {
		string query = "SELECT DISTINCT s.id, sf.field, sf.value FROM file s, file_field sf WHERE s.id = sf.file_id AND s.id IN (SELECT s2.id FROM file s2, file_field sf2 where s2.id = sf2.file_id " ;

		vector<string> bindFields;
		for (unsigned int i = 0; i < fileQuery.binaryTerms.size(); i++) {
			query += " AND ";
			BinaryTerm term = fileQuery.binaryTerms[i];
			if (term.field == "id") {
				query += " s2.id ";
			} else {
				query += " sf2.field = ? and sf2.value ";
				bindFields.push_back(term.field);
			}
			switch (term.op) {
				case equal:
					query += " = ";
					break;
				case not_equal: 
					query += " != ";
					break;
				case like: 
					query += " like ";
					break;
				default:
					throw new InvalidQueryException("unknown operator");
			}
			query += " ? ";
			bindFields.push_back(term.value);
		}
		query += ") ORDER BY s.id ASC";
		return rowsToFiles(this->db()->execute(query, bindFields));
	}
	shared_ptr<Files> FileStore::rowsToFiles(shared_ptr<Rows> theRows) {
		shared_ptr<Files> files(new Files);
		string lastID = "";
		shared_ptr<File> file;
		for (unsigned int i = 0; i < theRows->size(); i++) {
			Row row = (*theRows)[i];
			string id = row[0];
			string field = row[1];
			string value = row[2];
			if (id != lastID) {
				if (file.use_count()) {
					files->push_back(*file);
				}
				file.reset(new File);
				lastID = id;
			}
			if (field == ARTIST) 
				file->artist = value;
			else if (field == ALBUM) 
				file->album = value;
			else if (field == TITLE) 
				file->title = value;
			else if (field == GENRE) 
				file->genre = value;
			else if (field == YEAR) 
				file->year = value;
			else if (field == TRACK) 
				file->track = value;
			else if (field == PATH) 
				file->fileSystemPath = value;
		}
		if (file.use_count()) {
			files->push_back(*file);
		}
		return files;
	}

	bool FileStore::pathExists(const string & path) { 
		string query = "select * from file_field where field = ? and value = ?";
		vector<string> bind;
		bind.push_back(PATH);
		bind.push_back(path);
		shared_ptr<Rows> result = this->db()->execute(query, bind);
		return result->size() > 0;
	}
	void PlaylistStore::initialize() {
		this->db()->execute("create table if not exists playlist (id text primary key collate nocase, name text default '' collate nocase) ");
		this->db()->execute("create table if not exists playlist_file (id integer primary key, int position not null, file_id text collate nocase not null, playlist_id text collate nocase not null)");
		this->db()->execute("create index if not exists playlist_file_playlist_id_idx ON playlist_file(playlist_id)");
		this->db()->execute("create index if not exists playlist_file_file_id_idx ON playlist_file(file_id)");
	}

	void SQLiteDB::initialize() { 
		this->execute("pragma auto_vacuum = 1");
		this->execute("pragma encoding = \"UTF-8\"");
	}

	void FileStore::initialize() { 
		this->db()->execute("create table if not exists file (id text primary key collate nocase not null) ");
		this->db()->execute("create table if not exists file_field (id integer primary key not null, file_id text collate nocase, field text collate nocase not null, value text collate nocase not null)");
		this->db()->execute("create index if not exists file_field_value_idx ON file_field(value)");
		this->db()->execute("create index if not exists file_field_file_idx ON file_field(file_id)");
	}

	void PlaylistStore::add(const UUID& playlistID, const vector<UUID> & fileIDs) {
		for (unsigned int i = 0; i < fileIDs.size(); i++) {
			UUID fileID = fileIDs[i];
			string query = "insert into playlist (id, name) values (?, ?)";
			vector<string> bind;
			bind.push_back(playlistID);
			bind.push_back(fileID);
			this->db()->execute(query, bind);
		}
	}

	shared_ptr<UUID> PlaylistStore::create(const string& name) {
		shared_ptr<UUID> result = generateID();
		string query = "insert into playlist (id, name) values (?, ?)";
		vector<string> bind;
		bind.push_back(*result);
		bind.push_back(name);
		this->db()->execute(query, bind);
		return result;
	}
	void PlaylistStore::remove(const string& playlistID) {
		vector<string> bind;
		bind.push_back(playlistID);
		string query = "delete from playlist where id = ?";
		this->db()->execute(query, bind);
		query = "delete from playlist_file where playlist_id = ?";
		this->db()->execute(query, bind);
	}

	void FileStore::scan(const string & a_path) {
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
			if (this->pathExists(a_path))
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
				shared_ptr<UUID> fileID(create());
				this->addField(*fileID, ARTIST, artist);
				this->addField(*fileID, ARTIST, artist);
				this->addField(*fileID, ALBUM, album);
				this->addField(*fileID, YEAR, year);
				this->addField(*fileID, TITLE, title);
				this->addField(*fileID, GENRE, genre);
				this->addField(*fileID, TRACK, track);
				this->addField(*fileID, PATH, a_path);
			}
		}
	}

	shared_ptr<UUID> Model::generateID() {
		uuid_t *a_uuid;
		size_t sz = 0;
		uuid_create(&a_uuid);
		uuid_make(a_uuid, UUID_MAKE_V1);
		void *uuid_out = NULL;
		uuid_export(a_uuid, UUID_FMT_STR, &uuid_out, &sz);
		shared_ptr<UUID> result(new string((const char *)uuid_out, sz));
		free(uuid_out);
		uuid_destroy(a_uuid);
		return result;
	}

	shared_ptr<UUID> FileStore::create() {
		vector <string> bind;
		shared_ptr<UUID> fileID(generateID());
		bind.push_back(*fileID);
		string query = "insert into file (id) values (?)";
		this->db()->execute(query, bind);
		return fileID;
	}

	shared_ptr<Rows> SQLiteDB::execute(const string & query, vector<string> & bind) { 
		sqlite3_stmt *statement = NULL;
		cout << "query:" << query << endl;
		//cout << "bind:" << bind << endl;
		this->checkResultCode(sqlite3_prepare(this->db_, query.c_str(), -1, &statement, NULL));
		for (unsigned int i = 0; i < bind.size(); i++) {
			string s = bind[i];
			checkResultCode(sqlite3_bind_text(statement, i + 1, s.c_str(), s.size(), SQLITE_TRANSIENT));
		}
		shared_ptr<Rows> result(new Rows);
		int row_count = 0;
		while (true) {
			int result_code = checkResultCode(sqlite3_step(statement));
			if (result_code == SQLITE_ROW) {
				row_count += 1;
				int columns = sqlite3_column_count(statement);
				Row row;
				for (int i = 0; i < columns; i++) { 
					const char *bytes = (const char *)sqlite3_column_text(statement, i);
					string value = "";
					if (bytes != NULL) 
						value = bytes;
					cout << value << endl;
					row.push_back(value);
				}
				result->push_back(row);
			} else if (result_code == SQLITE_DONE) { 
				checkResultCode(sqlite3_finalize(statement));
				break;
			}
		}
		cout << "row count: " << row_count << endl;
		return result;
	}

	int SQLiteDB::checkResultCode(int result_code) {
		switch (result_code) {
			case SQLITE_OK:
			case SQLITE_DONE:
			case SQLITE_ROW:
				break;
			default:
				string msg = sqlite3_errmsg(db_);
				cout << "sqlite error: " << msg << endl;
				throw SQLite3Error(result_code);
		}
		return result_code;
	}

	void FileStore::addField(const string & fileID, const string & field, const string & value) {
		if (value.size() == 0)
			return;
		vector <string> bind;
		bind.push_back(fileID);
		bind.push_back(field);
		bind.push_back(value);
		string query = "insert into file_field (file_id, field, value) values (?, ?, ?)";
		this->db()->execute(query, bind);
	}

}



