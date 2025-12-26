OS_NAME := $(shell uname -s)
CC=gcc
ALL_SRC_FILES = $(wildcard *.c)

ifeq ($(OS_NAME), Darwin) # MACOS
	SRC_FILES := $(filter-out archke_event_loop_epoll.c, $(ALL_SRC_FILES))
endif
ifeq ($(OS_NAME), Linux) # Linux
	SRC_FILES := $(filter-out archke_event_loop_kqueue.c, $(ALL_SRC_FILES))
endif

.PHONY: clean cleand

all: exe

exe: $(SRC_FILES)
	$(CC) -Wall -c $^
	$(CC) *.o -o exe
	rm *.o

exed: $(SRC_FILES)
	$(CC) -Wall -c -g $^
	$(CC) *.o -o exed
	rm *.o

clean:
	rm -f *.o exe

cleand: 
	rm -f *.o exed

