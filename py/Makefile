all: lark/gen lark/gen/__init__.py

clean:
	rm -rf gen-py
	rm -rf lark/gen

lark/gen:
	rm -rf gen-py
	thrift -gen py:new_style ../interfaces/lark.thrift
	mv gen-py/lark/gen lark/gen
	rm -rf gen-py

lark/gen/__init__.py: lark/gen
	touch lark/gen/__init__.py
