BUILD_DIR = ./build
CFLAGS = -O3 -Wall -Wextra -pedantic --std=c11 `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0` 

all: x-selection-sync

x-selection-sync: src/main.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $< $(LDFLAGS) -o $(BUILD_DIR)/$@
