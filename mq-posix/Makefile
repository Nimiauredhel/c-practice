default:
	gcc *.c -lrt -o program
                                     
strict:                              
	bear -- gcc *.c -lrt -std=c99 -Wall -pedantic -Wextra -o program
                               
debug:                         
	gcc *.c -lrt -std=c99 -Wall -pedantic -Wextra -g -o0 -o program

run:
	./program

andrun:
	gcc *.c -o program
	./program

gdb:
	gdb ./program

valgrind:
	valgrind -s --leak-check=yes --track-origins=yes ./program

clean:
	rm program

