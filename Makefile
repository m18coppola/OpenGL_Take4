# -*-Makefile-*-

CC=gcc
FLAGS=`sdl2-config --libs --cflags` -W -Wall -g
LIBS= -lsoil2 -lGLEW -lGL -lm

all: main

main: main.o CoppLoader.o
	$(CC) main.o CoppLoader.o $(LIBS) $(FLAGS) -o app.bin

main.o: main.c
	$(CC) main.c $(LIBS) $(FLAGS) -c -o main.o

CoppLoader.o: CoppLoader.c
	$(CC) CoppLoader.c $(LIBS) $(FLAGS) -c -o CoppLoader.o

clean:
	rm -f *.o *.bin
