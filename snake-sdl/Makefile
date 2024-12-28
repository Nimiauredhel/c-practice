default:
	gcc *.c -lSDL2 -lSDL2main -lSDL2_ttf -lSDL2_mixer -o program
                                     
strict:                              
	bear -- gcc *.c -lSDL2 -lSDL2main -lSDL2_ttf -lSDL2_mixer -std=c99 -Wall -pedantic -Wextra -o program
                               
debug:                         
	gcc *.c -lSDL2 -lSDL2main -lSDL2_ttf -lSDL2_mixer -std=c99 -Wall -pedantic -Wextra -g -o0 -o program

run:
	./program

gdb:
	gdb ./program

valgrind:
	valgrind -s --leak-check=yes --track-origins=yes ./program

clean:
	rm program

