LIBS = -lthreads -lm

CC = gcc
CFLAGS = -std=c11 -c

all: main.o logging.o connection.o player.o
	$(CC) main.o logging.o player.o connection.o -o microMC $(LIBS)

main.o: main.c
	$(CC) $(CFLAGS) main.c $(LIBS)

logging.o: logging.c
	$(CC) $(CFLAGS) logging.c $(LIBS)

player.o: player.c
	$(CC) $(CFLAGS) player.c $(LIBS)

connection.o: connection.c
	$(CC) $(CFLAGS) connection.c $(LIBS)

clean:
	rm *.o microMC
