CC = zig cc
SRC = main.c
TARGET = binary

CFLAGS += -target aarch64-linux -std=gnu23
CFLAGS += -Oz -flto -s -static
CFLAGS += -fno-unwind-tables -fmerge-all-constants
CFLAGS += -Wl,--entry=__start
CFLAGS += -s -Wl,--gc-sections,-z,norelro
CFLAGS += -Wno-int-conversion -Wno-unknown-attributes -Wno-macro-redefined

all: $(TARGET) $(DUMP)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $@
	@if command -v sstrip >/dev/null 2>&1; then sstrip $(TARGET); fi

clean:
	rm -f $(TARGET) > /dev/null 2>&1

.PHONY: all clean
