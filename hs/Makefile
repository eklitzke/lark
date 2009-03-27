
all: gen

gen:
	rm -rf gen-hs
	thrift --gen hs ../interfaces/tmx.thrift
	mv gen-hs gen

clean:
	rm -rf gen gen-hs


