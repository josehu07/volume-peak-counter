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

counter-c: $(SRC_C)/counter.c
	gcc $< -o $(TARGET) -O2 -std=c99

.PHONY: clean
clean:
	rm -f counter
