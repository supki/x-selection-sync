BUILD_DIR = ./build
CFLAGS = -O3 -Wall -Wextra -pedantic --std=c11 `pkg-config --cflags x11` `pkg-config --cflags xfixes`
LDFLAGS = `pkg-config --libs x11` `pkg-config --libs xfixes`

all: x-selection-sync

x-selection-sync: src/main.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $< $(LDFLAGS) -o $(BUILD_DIR)/$@
