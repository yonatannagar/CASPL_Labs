# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
CC = gcc
CFLAGS  = -g -Wall -c

# All Targets
all: lab3

# Tool invocations
# Executable "lab3" depends on the files:
lab3: main.o toy_printf.o
	@echo 'Building target: lab3'
	$(CC) main.o toy_printf.o -o lab3
	@echo 'Finished building target: lab3'

# Depends on the source file (c)
main.o: main.c toy_stdio.h
	$(CC) $(CFLAGS) main.c -o main.o

toy_printf.o: toy_printf.c
	$(CC) $(CFLAGS) toy_printf.c -o toy_printf.o


# Clean the build directory and executable
clean:
	rm -f *.o lab3
