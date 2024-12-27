default:
	bear -- gcc *.c -lSDL2 -lSDL2main -o prog
                                     
strict:                              
	bear -- gcc *.c -lSDL2 -lSDL2main -std=c99 -Wall -pedantic -Wextra -o prog
                               
debug:                         
	bear -- gcc *.c -lSDL2 -lSDL2main -std=c99 -Wall -pedantic -Wextra -g -o0 -o prog

run:
	./prog

gdb:
	gdb ./prog

valgrind:
	valgrind -s --leak-check=yes --track-origins=yes ./prog

clean:
	rm prog

