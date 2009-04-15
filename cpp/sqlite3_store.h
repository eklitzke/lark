#include <iostream>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/lexical_cast.hpp>
#include <sqlite3.h>
#include "gen/lark_types.h"

#ifndef SQLITE3_STORE_H
#define SQLITE3_STORE_H

namespace lark {
	using namespace std;
	using namespace boost;

	typedef vector<string> Row;
	typedef vector<Row> Rows;
	typedef vector<File> Files;

	namespace fs = boost::filesystem;

	class InvalidQueryException: public std::exception {
		public:
			InvalidQueryException(string reason) {
				reason_ = reason;	
			}
			virtual ~InvalidQueryException() throw() {};

			virtual const char* what() const throw() {
				return reason_.c_str();
			}
		private:
			string reason_;
	};

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

	class SQLiteDB {
		public:
			SQLiteDB() : db_(NULL) {};
			virtual void connect(const string& databaseName) {
				this->checkResultCode(sqlite3_open(databaseName.c_str(), &db_));
				initialize();
			}
			virtual ~SQLiteDB() {
				if (db_ != NULL) {
					sqlite3_close(this->db_);
				}	
			}
			virtual int checkResultCode(int resultCode);
			virtual shared_ptr<Rows> execute(const string & query, vector<string> & bind);

			virtual shared_ptr<Rows> execute(const string & query) {
				vector<string> bind;
				return execute(query, bind);
			}
			virtual void initialize();

		private:
			sqlite3 *db_;
	};

	class Model {
		public:
			Model() { }
			virtual ~Model() {} 
			virtual shared_ptr<UUID> generateID();
			virtual void initialize() = 0;
			virtual void setDatabase(shared_ptr<SQLiteDB> db) { 
				db_ = db; 
				this->initialize(); 
			}
			virtual shared_ptr<SQLiteDB> db() { 
				return db_; 
			}
		private:
			shared_ptr<SQLiteDB> db_;
	};

	class FileStore : public Model {
		public:
			FileStore() : Model() { };
			~FileStore() {}
			virtual void scan(const string &path);
			virtual shared_ptr<Files> list(const FileQuery& fileQuery);
			virtual void addField(const UUID& fileID, const string& field, const string& value);
			virtual bool pathExists(const string & path); 
			void initialize();
			virtual shared_ptr<Files> rowsToFiles(shared_ptr<Rows>);
	};

	class SQLite3Store {
		public:
			SQLite3Store() : file_store_(new FileStore())  {} 
			
			void connect(const string & database_path) {
				shared_ptr<SQLiteDB> db(new SQLiteDB());
				db->connect(database_path);
				setDatabase(db);
			}

			virtual ~SQLite3Store() { }

			virtual void setDatabase(shared_ptr<SQLiteDB> db) {
				file_store_->setDatabase(db);
			}

			virtual shared_ptr<FileStore> file_store() { return file_store_; }
		private:
			shared_ptr<FileStore> file_store_;
	};
}
#endif

