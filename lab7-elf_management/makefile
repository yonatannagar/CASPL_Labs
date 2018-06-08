# All Targets
all: hexeditplus

# Tool invocations
# Executable "hexeditplus" depends on the files:
hexeditplus: hexeditplus.o 
	gcc hexeditplus.o -o hexeditplus
	rm -f *.o 

# Depends on the source file (c)
hexeditplus.o: task1.c 
	gcc -g -Wall -c task1.c -o hexeditplus.o


# Clean the build directory and executable
clean:
	rm -f *.o hexeditplus
