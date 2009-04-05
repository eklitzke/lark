CPPFLAGS = -L/opt/local/lib \
		   -L/usr/local/lib \
		   -I/usr/local/include/boost-1_38 \
		   -I/opt/local/include/taglib \
		   -I/opt/local/include/libavcodec \
		   -I/opt/local/include/libavformat \
		   -I/usr/local/include \
		   -I/opt/local/include \
		   -lboost_system-xgcc40-mt-1_38 \
		   -lboost_filesystem-xgcc40-mt-1_38 \
		   -lsqlite3 \
		   -luuid \
		   -ltag \
		   -lz \
		   -lm \
		   $(shell pkg-config --libs --cflags gstreamer-0.10) \
		   $(shell PKG_CONFIG_PATH=/opt/local/lib/pkgconfig:/usr/local/lib/pkgconfig pkg-config --libs --cflags gstreamer-0.10 thrift) \
		   -lavutil -lavcodec -lavformat

all: gen larkd

larkd_files = larkd.cpp \
			  indexer.cpp \
			  indexer.h \
			  lark_service_handler.h \
			  lark_service_handler.cpp \
			  player.cpp \
			  player.h \
			  gen/LarkService.cpp \
			  gen/LarkService.h \
			  gen/lark_constants.cpp \
			  gen/lark_constants.h \
			  gen/lark_types.cpp \
			  gen/lark_types.h

%.o: %.cpp %.h

larkd: larkd.o indexer.o player.o gen/lark_constants.o gen/lark_types.o gen/LarkService.o lark_service_handler.o

gen:
	rm -rf gen-cpp
	thrift --gen cpp ../interfaces/lark.thrift
	mv gen-cpp gen

clean:
	rm -rf gen gen-cpp larkd
