#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>

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

#ifndef INDEXER_H
#define INDEXER_H
using namespace std;
using namespace boost;

namespace fs = boost::filesystem;

string new_uuid();

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
		virtual bool path_exists(const string & path); 
	private:
		sqlite3 *db;
	
};

#endif

