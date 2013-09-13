LIBS = -lpthread -lm

CC = gcc
CFLAGS = -c

all: main.o logging.o player.o
	$(CC) main.o logging.o player.o -o microMC $(LIBS)

main.o: main.c
	$(CC) $(CFLAGS) main.c $(LIBS)

logging.o: logging.c
	$(CC) $(CFLAGS) logging.c $(LIBS)

player.o: player.c
	$(CC) $(CFLAGS) player.c $(LIBS)

clean:
	rm *.o microMC
