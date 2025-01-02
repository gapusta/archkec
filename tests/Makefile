.PHONY: clean cleand

hash_basic_tests: ../archke_kvstore_basic_hash.c archke_kvstore_basic_hash_tests.c
	gcc -Wall -I.. -c $^
	gcc *.o -o exe
	rm *.o

hash_basic_tests_debug: ../archke_kvstore_basic_hash.c archke_kvstore_basic_hash_tests.c
	gcc -Wall -I.. -c -g $^
	gcc *.o -o exed
	rm *.o

simple_string_tests: ../archke_simple_string_reader.c archke_simple_string_reader_tests.c
	gcc -Wall -I.. -c $^
	gcc *.o -o exe
	rm *.o

simple_string_tests_debug: ../archke_simple_string_reader.c archke_simple_string_reader_tests.c
	gcc -Wall -I.. -c -g $^
	gcc *.o -o exed
	rm *.o

binary_string_tests: ../archke_binary_string_reader.c archke_binary_string_reader_tests.c
	gcc -Wall -I.. -c $^
	gcc *.o -o exe
	rm *.o

binary_string_tests_debug: ../archke_binary_string_reader.c archke_binary_string_reader_tests.c
	gcc -Wall -I.. -c -g $^
	gcc *.o -o exed
	rm *.o

clean:
	rm exe

cleand: 
	rm exed

