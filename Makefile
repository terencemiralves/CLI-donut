CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wvla -std=c99 -pedantic

OBJ = main.o
BIN = donut

all: ${BIN}

${BIN}: ${OBJ}
	${CC} ${OBJ} -o ${BIN}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${BIN}
