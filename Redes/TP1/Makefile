CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2
BINDIR = bin

SRCS = server.c client.c
HDR = battle.h

all: dirs $(BINDIR)/server $(BINDIR)/client

dirs:
	mkdir -p $(BINDIR)

$(BINDIR)/server: server.c $(HDR)
	$(CC) $(CFLAGS) -o $@ server.c

$(BINDIR)/client: client.c $(HDR)
	$(CC) $(CFLAGS) -o $@ client.c

clean:
	rm -f $(BINDIR)/server $(BINDIR)/client

.PHONY: all clean dirs
