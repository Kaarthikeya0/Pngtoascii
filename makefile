CCW = /etc/alternatives/x86_64-w64-mingw32-gcc
CCL = gcc
CC = $(CCL)
CFLAGS+= -fsanitize=signed-integer-overflow -fsanitize=undefined -ggdb3 -O0 -std=c11 -Wall -Werror -Wextra -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable -Wshadow -Wpedantic
LDLIBS+= -lm -lc -lz

SRC_DIR = src
OBJ_DIR = obj

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))
BIN = asciiart.out

all: $(BIN)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LDLIBS)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ) $(LDLIBS)

clean:
	rm $(OBJ)
	rm $(BIN)

