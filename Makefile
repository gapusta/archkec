.PHONY: clean cleand

exe: $(wildcard *.c)
	gcc -Wall -c $^
	gcc *.o -o exe
	rm *.o

exed: $(wildcard *.c)
	gcc -Wall -c -g $^
	gcc *.o -o exed
	rm *.o

clean:
	rm *.o exe

cleand: 
	rm *.o exed

