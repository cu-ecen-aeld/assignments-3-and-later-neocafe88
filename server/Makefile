.DEFAULT_GOAL := aesdsocket

#CC=aarch64-none-linux-gnu-gcc
CFLAGS= -g -Wall  -pthread
CPPFLAGS=
LDFLAGS= -lpthread

all: aesdsocket

aesdsocket: aesdsocket.o 

clean:
	rm -rf *.o
	rm -rf aesdsocket
