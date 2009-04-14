
bin_PROGRAMS=larkd
larkd_CPPFLAGS = -Wall -I$(srcdir)/src $(BOOST_CPPFLAGS) $(THRIFT_CFLAGS) $(TAG_CFLAGS) $(GSTREAMER_CFLAGS) $(UUID_CFLAGS) $(SQLITE3_CFLAGS) $(UUID_CFLAGS)
larkd_LDFLAGS = -Wall $(BOOST_LDFLAGS) $(THRIFT_LIBS) $(TAG_LIBS) $(GSTREAMER_LIBS) $(SQLITE3_LIBS) $(UUID_LIBS) $(BOOST_THREAD_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_FILESYSTEM_LIB)
larkd_SOURCES=gen gen/lark_types.cpp gen/lark_constants.cpp gen/LarkService.cpp lark_service_handler.cpp player.cpp larkd.cpp sqlite3_store.cpp 

gen/lark_types.cpp: gen
gen/lark_types.h: gen
gen/lark_constants.cpp: gen
gen/lark_constants.h: gen
gen/LarkService.h: gen
gen/LarkService.cpp: gen

gen: ../interfaces/lark.thrift
	rm -rf gen
	rm -rf gen-cpp
	thrift --gen cpp $(srcdir)/../interfaces/lark.thrift
	mv gen-cpp @builddir@/gen

clean-local:
	rm -rf @builddir@/gen
	rm -rf @builddir@/gen-cpp

#clean:
#	rm -rf gen gen-cpp larkd
