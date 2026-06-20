# ─────────────────────────────────────────────────────────────
#  Canonical OpenGL Triangle – SDL3 / MinGW cross-compile
#
#  Usage:
#    make              – cross-compile for Windows
#    make run          – run under Wine
#    make clean        – remove build artifacts
#
# ─────────────────────────────────────────────────────────────

CC      = x86_64-w64-mingw32-gcc
TARGET  = triangle.exe

# Fedora mingw packages install here
SDL3_DIR ?= /usr/x86_64-w64-mingw32/sys-root/mingw

CFLAGS  = -std=c11 -Wall -Wextra -O2 \
           -I$(SDL3_DIR)/include \
           -I$(SDL3_DIR)/include/SDL3

# SDL3 static link + its Win32 deps
# -mwindows suppresses the console window; remove it to keep stdout visible
LDFLAGS = -L$(SDL3_DIR)/lib \
           -Wl,-Bstatic -lSDL3 -Wl,-Bdynamic \
           -lopengl32 \
           -lwinmm -lole32 -loleaut32 -limm32 -lsetupapi \
           -lversion -lgdi32 -ldinput8 -ldxguid -luuid -lshlwapi \
           -mwindows

.PHONY: all run clean

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

run: $(TARGET)
	wine $(TARGET)

clean:
	rm -f $(TARGET)
