

#include <stdio.h>
#include "toy_stdio.h"

int main(int argc, char *argv[]) {

    int counter = toy_printf("%s\n", "hello");toy_printf("printed: %d\n", counter);

    toy_printf("------------------------------------------\n");

    counter = toy_printf("octa 20: %o\n", 20);
    toy_printf("printed : %d\n", counter);
    counter = toy_printf("octa -1: %o\n", -1);
    toy_printf("printed %d\n", counter);
    counter = toy_printf("hexa 20: %x\n", 20);
    toy_printf("printed %d\n", counter);
    counter = toy_printf("hex -1: %x\n", -1);
    toy_printf("printed %d\n", counter);
    counter = toy_printf("HEXA 20: %X\n", 20);
    toy_printf("printed %d\n", counter);
    counter = toy_printf("HEXA -1: %X\n", -1);
    toy_printf("printed %d\n", counter);
    counter = toy_printf("bin 20: %b\n", 20);
    toy_printf("printed %d\n", counter);
    counter = toy_printf("bin -1: %b\n", -1);
    toy_printf("printed %d\n", counter);
    counter = toy_printf("unsigned int 20: %u\n", 20);
    toy_printf("printed %d\n", counter);
    counter = toy_printf("unsigned int -1: %u\n", -1);
    toy_printf("printed %d\n", counter);
    counter = toy_printf("signed int 20: %d\n", 20);
    toy_printf("printed %d\n", counter);
    counter = toy_printf("signed int -1: %d\n", -1);
    toy_printf("printed %d\n", counter);

    toy_printf("------------------------------------------\n");


    char * strings_array[] = {"This", "is", "array", "of", "strings"};
    char c_arr[] = {'a', 'd', 'c', 'b', 'z'};
    int integers_array[] = {1, 2, 3, 4, 5};
    int array_size = 5;
    counter = toy_printf("%Ad\n", integers_array, array_size);
    toy_printf("printed %d\n", counter);
    counter = toy_printf("%Ac\n", c_arr, array_size);
    toy_printf("printed %d\n", counter);
    counter = toy_printf("%As\n", strings_array, array_size);
    toy_printf("printed %d\n", counter);

    toy_printf("------------------------------------------\n");

    counter = toy_printf("Non-padded string: %s\n", "str");
    toy_printf("printed %d\n", counter);
  	counter = toy_printf("Right +11pad: %11s\n", "str");
    toy_printf("printed %d\n", counter);
  	counter = toy_printf("Left -6pad: %-6s\n", "str");
    toy_printf("printed %d\n", counter);
  	counter = toy_printf("Zeroes 5pad: %05d\n", -1);
    toy_printf("printed %d\n", counter);

}