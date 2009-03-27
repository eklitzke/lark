
all: gen

gen:
	rm -rf gen-cpp
	thrift --gen cpp ../interfaces/tmx.thrift
	mv gen-cpp gen

clean:
	rm -rf gen gen-cpp


