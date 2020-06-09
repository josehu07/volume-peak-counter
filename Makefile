SRC_C=src-c
SRC_RS=src-rs
SRC_GO=src-go

TARGET=counter


.PHONY: all
all:
	@echo "Please specify one of the following source language versions:"
	@echo "  make counter-c"
	@echo "  make counter-rs"
	@echo "  make counter-go"

ring-buf.o: $(SRC_C)/ring-buf.c $(SRC_C)/ring-buf.h
	gcc $< -c -o $@ -O2 -std=c99

counter-c: $(SRC_C)/counter.c ring-buf.o gnuplot_i/gnuplot_i.o
	@if [ ! -e "loudness-scanner/build/loudness" ]; then 				\
		@echo "ERROR: 'loudness-scanner' is not correctly prepared."; 	\
	elif [ ! -e "gnuplot_i/gnuplot_i.o" ]; then 						\
		@echo "ERROR: 'gnuplot_i' is not correctly prepared."; 			\
	fi;
	gcc $^ -o $(TARGET) -O2 -std=c99

.PHONY: clean
clean:
	rm -f *.o $(TARGET) gnuplot_tmpdata*
