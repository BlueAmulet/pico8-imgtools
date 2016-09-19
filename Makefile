CC=gcc
CFLAGS=-Os

all: p8relabel p8split

lodepng.c:
	@echo "Downloading lodepng from GitHub ..."
	@wget -q --show-progress -O lodepng.c "https://raw.githubusercontent.com/lvandeve/lodepng/master/lodepng.cpp"
	@wget -q --show-progress -O lodepng.h "https://raw.githubusercontent.com/lvandeve/lodepng/master/lodepng.h"

p8relabel: p8relabel.c lodepng.c
	$(CC) -o p8relabel p8relabel.c lodepng.c

p8split: p8split.c lodepng.c
	$(CC) -o p8split p8relabel.c lodepng.c

.PHONY: clean

clean:
	rm -f p8relabel p8split lodepng.c lodepng.h
