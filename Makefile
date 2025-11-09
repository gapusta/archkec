CC=gcc
SRC_FILES = $(wildcard *.c)
LINUX_SRC_FILES = $(filter-out archke_event_loop_kqueue.c, $(SRC_FILES))
FREE_BSD_SRC_FILES = $(filter-out archke_event_loop_epoll.c, $(SRC_FILES))

.PHONY: clean cleand

all: exe

exe: $(LINUX_SRC_FILES)
	$(CC) -Wall -c $^
	$(CC) *.o -o exe
	rm *.o

exed: $(LINUX_SRC_FILES)
	$(CC) -Wall -c -g $^
	$(CC) *.o -o exed
	rm *.o

exe_freebsd: $(FREE_BSD_SRC_FILES)
	$(CC) -Wall -c $^
	$(CC) *.o -o exe
	rm *.o

exed_freebsd: $(FREE_BSD_SRC_FILES)
	$(CC) -Wall -c -g $^
	$(CC) *.o -o exed
	rm *.o

clean:
	rm -f *.o exe

cleand: 
	rm -f *.o exed

