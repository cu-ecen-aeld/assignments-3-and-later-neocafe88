.DEFAULT_GOAL := aesdsocket

#CC=aarch64-none-linux-gnu-gcc
CC ?= $(CORSS_COMPILE}gcc
CFLAGS ?= -g -Wall -Werror
TARGET ?= aesdsocket
LDFLAGS ?= -lpthread -lrt

all: aesdsocket

aesdsocket: aesdsocket.c 
	$(CC) $(CFLAGS) $^ -o $@ $(INCLUDES) $(LDFLAGS)

clean:
	rm -rf *.o
	rm -rf aesdsocket
