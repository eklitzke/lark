#include <tag.h>
#include <fileref.h>

#include "sqlite3_store.h"

namespace lark {
	static const string URI = "path";
	static const string ARTIST = "artist";
	static const string ALBUM = "album";
	static const string TITLE = "title";
	static const string GENRE = "genre";
	static const string TRACK = "track";
	static const string YEAR = "year";

	shared_ptr<Files> FileStore::list(const FileQuery & fileQuery) {
		string query = "SELECT sf.file_id, sf.field, sf.value FROM file_field sf WHERE sf.file_id IN (SELECT sf2.file_id FROM file_field sf2 ";

		vector<string> bindFields;
		for (unsigned int i = 0; i < fileQuery.binaryTerms.size(); i++) {
			if (i > 0)
				query += " AND ";
			else 
				query += " WHERE "; 

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
		query += ") ORDER BY sf.file_id ASC";
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
			else if (field == URI) 
				file->uri = value;
		}
		if (file.use_count()) {
			files->push_back(*file);
		}
		return files;
	}

	bool FileStore::pathExists(const string & path) { 
		string query = "select * from file_field where field = ? and value = ?";
		vector<string> bind;
		bind.push_back(URI);
		bind.push_back(path);
		shared_ptr<Rows> result = this->db()->execute(query, bind);
		return result->size() > 0;
	}
	void SQLiteDB::initialize() { 
		this->execute("pragma auto_vacuum = 1");
		this->execute("pragma encoding = \"UTF-8\"");
	}

	void FileStore::initialize() { 
		this->db()->execute("create table if not exists file_field (id integer primary key not null, file_id text collate nocase, field text collate nocase not null, value text collate nocase not null)");
		this->db()->execute("create index if not exists file_field_value_idx ON file_field(value)");
		this->db()->execute("create index if not exists file_field_file_idx ON file_field(file_id)");
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
				cerr << "indexing " << a_path << endl;
				TagLib::Tag *t = file_ref.tag();
				string artist = t->artist().to8Bit(true);
				string album = t->album().to8Bit(true);
				string title = t->title().to8Bit(true);
				string genre = t->genre().to8Bit(true);
				string year = t->year() > 0 ? lexical_cast<string>(t->year()) : "";
				string track = t->track() > 0 ? lexical_cast<string>(t->track()) : "";
				string uri = "file://" + a_path;
				shared_ptr<UUID> fileID(generateID());
				this->addField(*fileID, ARTIST, artist);
				this->addField(*fileID, ARTIST, artist);
				this->addField(*fileID, ALBUM, album);
				this->addField(*fileID, YEAR, year);
				this->addField(*fileID, TITLE, title);
				this->addField(*fileID, GENRE, genre);
				this->addField(*fileID, TRACK, track);
				this->addField(*fileID, URI, uri);
			}
		}
	}

	shared_ptr<UUID> Model::generateID() {
		// Generate a pseudo random 128 bit number and hex encode it into a string
		FILE *urandom = fopen("/dev/urandom", "r");
		char output[32];
		for (int i = 0; i < 16; i++) {
			int8_t num;
			while (fread(&num, 1, 1, urandom) <= 0) {
				;
			}
			sprintf(output + (i * 2), "%02X", num);
		}
		fclose(urandom);
		shared_ptr<UUID> result(new string(output, 32));
		return result;
	}

	shared_ptr<Rows> SQLiteDB::execute(const string & query, vector<string> & bind) { 
		sqlite3_stmt *statement = NULL;
		//cout << "query:" << query << endl;
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
					row.push_back(value);
				}
				result->push_back(row);
			} else if (result_code == SQLITE_DONE) { 
				checkResultCode(sqlite3_finalize(statement));
				break;
			}
		}
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
				cerr << "sqlite error: " << msg << endl;
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



