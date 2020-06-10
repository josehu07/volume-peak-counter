TARGET=counter


.PHONY: all
all:
	@echo "Please specify one of the following source language versions:"
	@echo "  make counter-c"
	@echo "  make counter-rs"
	@echo "  make counter-go"
	@echo "  make counter-py"


define check_loudness
	@if [ ! -e "loudness-scanner/build/loudness" ]; then 				\
		@echo "ERROR: 'loudness-scanner' is not correctly prepared."; 	\
		exit 1; 														\
	fi;
endef


# C version.
CC=gcc
CFLAGS=-O2 -std=c99 -g

ring-buf.o: src-c/ring-buf.c src-c/ring-buf.h
	$(CC) $(CFLAGS) $< -c -o $@

gnuplot_i.o: src-c/gnuplot_i.c src-c/gnuplot_i.h
	$(CC) $(CFLAGS) $< -c -o $@

counter-c: src-c/counter.c ring-buf.o gnuplot_i.o
	$(check_loudness)
	$(CC) $(CFLAGS) $^ -o $(TARGET)


# Rust version.
counter-rs:
	@echo "NOT IMPLEMENTED!"


# Go version.
counter-go:
	@echo "NOT IMPLEMENTED!"


# Python version (a py3 script with hashbang).
counter-py:
	@echo "NOT IMPLEMENTED!"


.PHONY: clean
clean:
	rm -f *.o $(TARGET) gnuplot_tmpdata*
