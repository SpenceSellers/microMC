LIBS = -lpthread -lm

CC = gcc
CFLAGS = -c

OBJS = main.o logging.o connection.o player.o server.o map.o packets.o

BINARY= microMC

all: $(OBJS)
	$(CC) $(OBJS) -o $(BINARY) $(LIBS)

%.o:
	$(CC) $(CFLAGS) $*.c $(LIBS)

clean:
	rm *.o $(BINARY)
