CC=gcc
LD=gcc
CFLAGS= -Wall -ggdb 
LIBS= -lm

OBJ_bmpmod=bmpmod.o\
	bmp.o\
	filter.o

bmpmod : $(OBJ_bmpmod)
	$(LD) -o bmpmod $(OBJ_bmpmod) $(LIBS)

bmpmod.o : bmpmod.c
	$(CC) $(CFLAGS) -c bmpmod.c -o bmpmod.o

bmp.o : bmp.c	bmp.h
	$(CC) $(CFLAGS) -c bmp.c -o bmp.o

filter.o : filter.c	filter.h
	$(CC) $(CFLAGS) -c filter.c -o filter.o
	
clean:
	rm *.o
	rm bmpmod
