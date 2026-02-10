# Makefile para o EP1 (Pthreads) - atualizado para expor clock_gettime/nanosleep
CC = gcc
# define POSIX feature macro para expor CLOCK_REALTIME, nanosleep, clock_gettime, etc.
CFLAGS = -std=c11 -Wall -Wextra -O2 -g -D_POSIX_C_SOURCE=199309L
LDFLAGS = -pthread

SRC = ep1.c passa_tempo.c
OBJ = $(SRC:.c=.o)
TARGET = ep1

.PHONY: build clean run

build: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJ)
