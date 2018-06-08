#include "toy_stdio.h"


int main(int argc, char *argv[]) {/*
    int chars_printed = toy_printf("%x, %X\\n", 496351, 496351);
    chars_printed += toy_printf("Welcome to \\c\\n");
    chars_printed += toy_printf("Support for explicit\\N");
    toy_printf("Printed %d chars\n", chars_printed); */

    toy_printf("octa 20: %o\n", 20);
    toy_printf("octa -1: %o\n", -1);
    toy_printf("hexa 20: %x\n", 20);
    toy_printf("hexa -1: %x\n", -1);
    toy_printf("HEXA 20: %X\n", 20);
    toy_printf("HEXA -1: %X\n", -1);    
    toy_printf("bin 20: %b\n", 20);
    toy_printf("bin -1: %b\n", -1);
    toy_printf("unsigned int 20: %u\n", 20);
    toy_printf("unsigned int -1: %u\n", -1);
    toy_printf("signed int 20: %d\n", 20);
    toy_printf("signed int -1: %d\n", -1);

    
    int integers_array[] = {1,2,3,4,5};
  	char * strings_array[] = {"This", "is", "array", "of", "strings"};
  	int array_size = 5;
  	toy_printf("Print array of integers: %Ad\n", integers_array, array_size);
  	toy_printf("Print array of strings: %As\n", strings_array, array_size);

  	toy_printf("\nsize of char*: %d\n", sizeof(char*));
  	toy_printf("size of void*: %d\n\n", sizeof(void*));

  	toy_printf("Non-padded string: %s\n", "str");
  	toy_printf("Right-padded string: %6s\n", "str");
  	toy_printf("Left-added string: %-6s\n", "str");
  	toy_printf("With numeric placeholders: %05d\n", -1);
    
    char w[] = "words";
    toy_printf("\nSuggested fix for task0:\n");
    toy_printf("print string words: %s\n", w); 
    w[4] = 0;//NULL the last slot of the array
    toy_printf("print string word: %s\n", w);
}