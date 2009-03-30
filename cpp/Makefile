LOCALE = en_US

CPPFLAGS = -L/usr/local/lib \
		   -I/usr/local/include/boost-1_38 \
		   -I/opt/local/include/taglib \
		   -I/usr/local/include \
		   -I/opt/local/include \
		   -lboost_system-xgcc40-mt-1_38 \
		   -lboost_filesystem-xgcc40-mt-1_38 \
		   -L/opt/local/lib \
		   -lsqlite3 \
		   -luuid \
		   -ltag \
		   -ggdb

all: gen indexer

indexer: indexer.cpp

gen:
	rm -rf gen-cpp
	thrift --gen cpp ../interfaces/tmx.thrift
	mv gen-cpp gen

clean:
	rm -rf gen gen-cpp indexer
