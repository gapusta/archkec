.PHONY: clean cleand

hash_basic_tests: ../archke_kvstore_basic_hash.c archke_kvstore_basic_hash_tests.c
	gcc -Wall -I.. -c $^
	gcc *.o -o exe
	rm *.o

hash_basic_tests_debug: ../archke_kvstore_basic_hash.c archke_kvstore_basic_hash_tests.c
	gcc -Wall -I.. -c -g $^
	gcc *.o -o exed
	rm *.o

client_tests: ../archke_server.c archke_client_tests.c
	gcc -Wall -I.. -c $^
	gcc *.o -o exe
	rm *.o

client_tests_debug: ../archke_server.c archke_client_tests.c
	gcc -Wall -I.. -c -g $^
	gcc *.o -o exed
	rm *.o

echo_cmd_tests: ../archke_commands.c archke_echo_command_tests.c ../archke_kvstore_basic_hash.c ../archke_error.c
	gcc -Wall -I.. -c $^
	gcc *.o -o exe
	rm *.o

echo_cmd_tests_debug: ../archke_commands.c archke_echo_command_tests.c ../archke_kvstore_basic_hash.c ../archke_error.c
	gcc -Wall -I.. -c -g $^
	gcc *.o -o exed
	rm *.o

clean:
	rm exe

cleand: 
	rm exed

