PROGRAM=program
SRC= main.c src/*.c
TESTS= tests/check_main.c src/*.c 
ARGS=

default:
	gcc $(SRC) -o $(PROGRAM).o
                                     
strict:                              
	bear -- gcc $(SRC) -std=c99 -Wall -pedantic -Wextra -o $(PROGRAM).o
                    
debug:              
	gcc $(SRC) -std=c99 -Wall -pedantic -Wextra -g -o0 -o $(PROGRAM).o

run:
	./$(PROGRAM).o $(ARGS)

andrun:
	gcc *.c -o $(PROGRAM).o
	./$(PROGRAM).o $(ARGS)

gdb:
	gdb ./$(PROGRAM).o $(ARGS)

valgrind:
	valgrind -s --leak-check=yes --track-origins=yes ./$(PROGRAM).o $(ARGS)

test:
	gcc $(TESTS) -o $(PROGRAM)_tests.o -lcheck -lsubunit -lrt -lm -lpthread
	./$(PROGRAM)_tests.o

clean:
	rm -f $(PROGRAM).o
	rm -f compile_commands.json

