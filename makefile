LIBS = -lpthread -lm -lz

CC = gcc

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS = -c -g -DDEBUG
else
	CFLAGS = -c
endif


OBJS = main.o logging.o connection.o player.o server.o map.o packets.o encodings.o \
	playerconnectionhandlers.o inventory.o

BINARY= microMC

all: $(OBJS)
	$(CC) $(OBJS) -o $(BINARY) $(LIBS)

%.o:
	$(CC) $(CFLAGS) $*.c $(LIBS)

clean:
	rm *.o $(BINARY)

