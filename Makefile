bin/fastest_thread: fastest_thread.c
	gcc -o bin/fastest_thread fastest_thread.c -lm
bin/fastest_thread_debug: fastest_thread.c
	gcc -g -O0 -o bin/fastest_thread_debug fastest_thread.c -lm
