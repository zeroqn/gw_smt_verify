CC := gcc
LD := gcc
CFLAGS := -I deps

all: main

main: main.c deps/gw_def.h deps/gw_smt.h deps/impl.c
	$(CC) $(CFLAGS) -o $@ $<
