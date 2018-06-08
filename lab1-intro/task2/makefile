# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
CC = gcc
CFLAGS  = -g -Wall -c

# All Targets
all: wordCounter

# Tool invocations
# Executable "wordCounter" depends on the files:
wordCounter: wordCounter.o
	@echo 'Building target: wordCounter'
	$(CC) wordCounter.o -o wordCounter
	@echo 'Finished building target: wordCounter'

# Depends on the source file (c)
wordCounter.o: wordCounter.c
	$(CC) $(CFLAGS) -o wordCounter.o wordCounter.c


# Clean the build directory and executable
clean:
	rm -f *.o wordCounter
