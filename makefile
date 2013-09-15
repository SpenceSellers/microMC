LIBS = -lpthread -lm

CC = gcc
CFLAGS = -c

all: main.o logging.o connection.o player.o server.o map.o packets.o
	$(CC) main.o logging.o player.o connection.o \
	server.o map.o packets.o \
	-o microMC $(LIBS)

main.o: main.c
	$(CC) $(CFLAGS) main.c $(LIBS)

logging.o: logging.c
	$(CC) $(CFLAGS) logging.c $(LIBS)

player.o: player.c
	$(CC) $(CFLAGS) player.c $(LIBS)

connection.o: connection.c
	$(CC) $(CFLAGS) connection.c $(LIBS)

server.o: server.c
	$(CC) $(CFLAGS) server.c $(LIBS)

map.o: map.c
	$(CC) $(CFLAGS) map.c $(LIBS)

packets.o: packets.c
	$(CC) $(CFLAGS) packets.c $(LIBS)

clean:
	rm *.o microMC
