# Static recompilation of Warlocks: the translated program (gen/, from
# SuperSpider's analyse and gen) and game.c, linked with libroger (the RISC
# OS runtime).
ROGER   ?= $(HOME)/stuff/libroger
SS      ?= $(HOME)/stuff/superspider
SSBIN   := $(SS)/_build/default/bin/main.exe
CC      ?= gcc
CFLAGS  ?= -O2 -g -w -fno-strict-aliasing $(if $(TRACE),-DRECOMP_TRACE)
GEN     := $(wildcard gen/code*.c) gen/entries.c
OBJ     := $(GEN:.c=.o) game.o
LIBS    := $(shell pkg-config --libs sdl3) -lm -lpthread
INC     := -I$(ROGER)/include
# !Runimage (so spelt on the disc), whatever its ,load-exec suffix
IMAGE   := $(firstword $(wildcard ../game/!Warlocks/!Runimage,*) ../game/!Warlocks/!Runimage)

warlocks: $(OBJ) $(ROGER)/libroger.a
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(ROGER)/libroger.a $(LIBS)

$(ROGER)/libroger.a: FORCE
	$(MAKE) -C $(ROGER) libroger.a

gen/%.o: gen/%.c $(ROGER)/include/roger_cpu.h gen/gen.h
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

game.o: game.c $(ROGER)/include/roger.h
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

# make regen: !Runimage with WM unpacked into it (../tools/unpack: the
# image that actually runs, ../unpacked/!Runimage), analysed (its roots:
# ../analysis/warlocks.roots) and translated. The game still loads the
# original !Runimage, and unpacks WM over it itself.
UNPACKED := ../unpacked/!Runimage
regen:
	cd $(SS) && dune build
	$(MAKE) -C ../tools ROGER=$(ROGER)
	mkdir -p ../unpacked
	cd ../tools && ./unpack '$(IMAGE)' --stop=b52c --write='$(UNPACKED)' --limit=50000000
	touch ../analysis/warlocks.roots
	$(SSBIN) analyse '$(UNPACKED)' 8000 --full --roots ../analysis/warlocks.roots -o ../analysis/warlocks.json
	rm -rf gen
	$(SSBIN) gen '$(UNPACKED)' 8000 ../analysis/warlocks.json --c gen

# make roots: merge the gap entries a --trace-gaps run logged
# (untranslated.log, or LOG=...) into ../analysis/warlocks.roots
LOG ?= untranslated.log
roots:
	python3 -I $(SS)/tools/gap_roots.py '$(UNPACKED)' 8000 ../analysis/warlocks.roots $(LOG)

clean:
	rm -f gen/*.o game.o warlocks

.PHONY: FORCE regen roots clean
