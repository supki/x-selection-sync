BUILD_DIR = ./build
CFLAGS = -O3 -Wall -Wextra -pedantic --std=gnu11 `pkg-config --cflags x11` `pkg-config --cflags xfixes`
LDFLAGS = `pkg-config --libs x11` `pkg-config --libs xfixes`

STRIP = strip

all: $(BUILD_DIR)/x-selection-sync

$(BUILD_DIR)/x-selection-sync: src/main.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $< $(LDFLAGS) -o $@
	$(STRIP) $@
