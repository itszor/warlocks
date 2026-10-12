# tools/unpack: !Runimage as it runs, with WM unpacked into it (see unpack.c).
ROGER ?= $(HOME)/stuff/libroger
CC    ?= gcc

unpack: unpack.c $(ROGER)/libroger.a $(ROGER)/include/roger.h
	$(CC) -O2 -g -Wall -I$(ROGER)/include -o $@ unpack.c $(ROGER)/libroger.a $(shell pkg-config --libs sdl3) -lm -lpthread

clean:
	rm -f unpack

.PHONY: clean
