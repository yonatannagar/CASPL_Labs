# All Targets
all: client

# Tool invocations
# Executable "client" depends on the files:
client: client.o line_parser.o common.o
	gcc client.o common.o line_parser.o -o client
	rm -f *.o 


# Depends on the source file (c)
client.o: client.c line_parser.h common.h
	gcc -g -Wall -c client.c -o client.o
common.o: common.c common.h 
	gcc -g -Wall -c common.c -o common.o
line_parser.o: line_parser.c line_parser.h
	gcc -g -Wall -c line_parser.c -o line_parser.o
# Clean the build directory and executable
clean:
	rm -f *.o client
