all: gen gen/__init__.py

clean:
	rm -rf gen

gen:
	rm -rf gen-py
	thrift -gen py:new_style ../interfaces/tmx.thrift
	mv gen-py gen

gen/__init__.py: gen
	touch gen/__init__.py
