CC=gcc
CFLAGS= -Wall -lxcb

all:
	$(CC) -o baconwm baconwm.c $(CFLAGS)
