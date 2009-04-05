#import "indexer.h"

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
		indexer.scan(s);
	}
	cout << endl;

	return 0;
}


