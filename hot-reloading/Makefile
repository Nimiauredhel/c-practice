PROGRAM=program.o
BUILD_DIR=build/
BUILD_PATH=$(BUILD_DIR)$(PROGRAM)
ARGS=

default: dynamic.so
	mkdir -p $(BUILD_DIR)
	gcc main.c -o $(BUILD_PATH)
                                     
strict: dynamic.so                              
	mkdir -p $(BUILD_DIR)
	bear -- gcc main.c -std=c99 -Wall -pedantic -Wextra -o $(BUILD_PATH)
                    
debug: dynamic.so              
	mkdir -p $(BUILD_DIR)
	gcc main.c -std=c99 -Wall -pedantic -Wextra -g -o0 -o $(BUILD_PATH)

dynamic.so:
	mkdir -p $(BUILD_DIR)
	gcc -shared -o $(BUILD_DIR)dynamic.so -fPIC dynamic.c

.ONESHELL:
run:
	cd $(BUILD_DIR)
	./$(PROGRAM) $(ARGS)

andrun:
	make default
	make run

gdb:
	cd $(BUILD_DIR)
	gdb $(BUILD_PATH) $(ARGS)

valgrind:
	cd $(BUILD_DIR)
	valgrind -s --leak-check=yes --track-origins=yes $(BUILD_PATH) $(ARGS)

clean:
	rm -rf $(BUILD_DIR)

