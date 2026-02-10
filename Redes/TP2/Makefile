CC = gcc
CFLAGS = -Wall -pthread
DEPS = common.h

all: server cliente

server: server.c $(DEPS)
	$(CC) -o server server.c $(CFLAGS)

client: cliente.c $(DEPS)
	$(CC) -o cliente cliente.c $(CFLAGS)

clean:
	rm -f server cliente