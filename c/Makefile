CC := gcc
LD := gcc
CFLAGS := -I deps

all: main

main: main.c deps/gw_def.h
	$(CC) $(CFLAGS) -o $@ $<
