COMPILER = gcc
C_FLAGS = -g -w

C_FILES = $(shell find -name '*.c')

default: run

clean:
	rm -f *.o

run: myshell
	./a

myshell: $(C_FILES)
	$(COMPILER) $(C_FLAGS) $(C_FILES) -o a