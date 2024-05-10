CC = gcc
CFLAGS+= -fsanitize=signed-integer-overflow -fsanitize=undefined -ggdb3 -O0 -std=gnu11 -Wall -Werror -Wextra -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable -Wshadow 
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

pngtest:
	gcc $(CFLAGS) -c tst/test_pngtests.c -o obj/test_pngtests.o 
	gcc $(CFLAGS) obj/pngtests.o obj/test_pngtests.o obj/gettingbyte.o obj/getchunkdata.o -o test_pngtests.out

clean:
	rm $(OBJ)
	rm $(BIN)

