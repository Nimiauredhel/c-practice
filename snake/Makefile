default:
	bear -- gcc *.c -o prog

strict:
	bear -- gcc  *.c -std=c99 -Wall -pedantic -Wextra -o prog
                            
debug:                      
	bear -- gcc  *.c -std=c99 -Wall -pedantic -Wextra -g -o0 -o prog

run:
	./prog

gdb:
	gdb ./prog

valgrind:
	valgrind -s --leak-check=yes --track-origins=yes ./prog

clean:
	rm prog

