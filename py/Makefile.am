all: lark/gen lark/gen/__init__.py

all-local: lark/gen

clean-local:
	rm -rf gen-py
	rm -rf lark/gen

lark/gen: $(srcdir)/../interfaces/lark.thrift
	rm -rf $(srcdir)/gen-py
	thrift -gen py:new_style $(srcdir)/../interfaces/lark.thrift
	mv gen-py/lark/gen $(srcdir)/lark/gen
	rm -rf gen-py

lark/gen/__init__.py: lark/gen
	touch lark/gen/__init__.py
