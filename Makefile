CC = gcc
TARGET = coursework
FLAGS = -g2 -ggdb -std=c11 -pedantic -W -Wall -Wextra
GTK_INCLUDE = `pkg-config --cflags gtk+-3.0`
GTK_LIBS = `pkg-config --libs gtk+-3.0`

PREF_SRC = ./src/
BUILD = ./build
DEBUG = $(BUILD)/debug/
RELEASE = $(BUILD)/release
OUT_OBJ = $(DEBUG)

SRC = $(wildcard $(PREF_SRC)*.c)
OBJ = $(patsubst $(PREF_SRC)%.c, $(OUT_OBJ)%.o, $(SRC))

vpath %.c ./src/
vpath %.h ./src/
vpath %.o build/debug

ifeq ($(MODE), debug)
    CFLAGS = -g2 -ggdb -std=c11 -pedantic -W -Wall -Wextra
    OUT_DIR = $(DEBUG)
    vpath %.o build/debug
else ifeq ($(MODE), release)
    CFLAGS = -std=c11 -pedantic -W -Wall -Wextra -Werror
    OUT_DIR = $(RELEASE)
    vpath %.o build/release
endif

$(TARGET): $(OBJ)
	$(CC) $(FLAGS) $(GTK_INCLUDE) $(OBJ) -o $(RELEASE)/$(TARGET) -lncursesw -lncurses -lform $(GTK_LIBS)

$(OUT_OBJ)%.o: $(PREF_SRC)%.c
	$(CC) $(FLAGS) $(GTK_INCLUDE) -c $< -o $@

.PHONY: all
all: clean $(DEBUG) $(RELEASE) $(TARGET)

$(DEBUG):
	mkdir -p $(DEBUG)

$(RELEASE):
	mkdir -p $(RELEASE)

.PHONY: clean
clean:
	@rm -rf $(BUILD)

.PHONY: run
run:
	./$(RELEASE)/$(TARGET)