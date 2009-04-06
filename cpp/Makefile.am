
bin_PROGRAMS=larkd
larkd_CPPFLAGS = -Wall -I$(srcdir)/src $(BOOST_CPPFLAGS) $(THRIFT_CFLAGS) $(TAG_CFLAGS) $(GSTREAMER_CFLAGS) $(UUID_CFLAGS) $(SQLITE3_CFLAGS) $(UUID_CFLAGS)
larkd_LDFLAGS = -Wall $(BOOST_LDFLAGS) $(THRIFT_LIBS) $(TAG_LIBS) $(GSTREAMER_LIBS) $(SQLITE3_LIBS) $(UUID_LIBS) $(BOOST_THREAD_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_FILESYSTEM_LIB)
larkd_SOURCES=larkd.cpp gen/lark_types.cpp gen/lark_constants.cpp gen/LarkService.cpp lark_service_handler.cpp player.cpp indexer.cpp 

#gen:
#	rm -rf gen-cpp
#	thrift --gen cpp ../interfaces/lark.thrift
#	mv gen-cpp gen

#clean:
#	rm -rf gen gen-cpp larkd
