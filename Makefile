all:
	$(MAKE) -C py
	$(MAKE) -C cpp

clean:
	$(MAKE) -C py clean
	$(MAKE) -C cpp clean

