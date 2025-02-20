.PHONY: clean cleand

hash_basic_tests: ../archke_tests.c ../archke_kvstore_basic_hash.c archke_kvstore_basic_hash_tests.c
	gcc -Wall -I.. -c $^
	gcc *.o -o exe
	rm *.o

client_tests: ../archke_tests.c ../archke_server.c archke_client_tests.c ../archke_error.c
	gcc -Wall -I.. -c $^
	gcc *.o -o exe
	rm *.o

clean:
	rm exe

cleand: 
	rm exed

