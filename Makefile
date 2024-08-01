CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wvla -std=c99 -pedantic -D_POSIX_C_SOURCE=199309L -g


OBJ = main.o render.o
BIN = donut

all: ${BIN}

${BIN}: ${OBJ}
	${CC} ${OBJ} -fsanitize=address -o ${BIN} -lm

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${BIN}
