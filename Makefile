CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wvla -std=c99 -pedantic -D_POSIX_C_SOURCE=199309L

OBJ = main.o
BIN = donut

all: ${BIN}

${BIN}: ${OBJ}
	${CC} ${OBJ} -o ${BIN}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${BIN}
