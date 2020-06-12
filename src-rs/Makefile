TARGET=counter


RSC=cargo build
RSFLAGS=--release


.PHONY: all
all:
	$(RSC) $(RSFLAGS)
	@if [ "$$?" -eq "0" ]; then 			\
		mv target/release/$(TARGET) ..;		\
	fi
