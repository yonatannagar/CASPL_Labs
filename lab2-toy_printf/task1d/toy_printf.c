

/* toy-io.c
 * Limited versions of printf and scanf
 *
 * Programmer: Mayer Goldberg, 2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>

/* the states in the printf state-machine */
enum printf_state {
    st_printf_init,
    st_printf_percent,
    st_printf_array,
    st_printf_width_field
};

#define MAX_NUMBER_LENGTH 64
#define is_octal_char(ch) ('0' <= (ch) && (ch) <= '7')

int toy_printf(char *fs, ...);

const char *digit = "0123456789abcdef";
const char *DIGIT = "0123456789ABCDEF";

int print_int_helper(int n, int radix, const char *digit) {
    int result;

    if (n < radix) {
        putchar(digit[n]);
        return 1;
    }
    else {
        result = print_int_helper(n / radix, radix, digit);
        putchar(digit[n % radix]);
        return 1 + result;
    }
}
int print_unsigned_int_helper(unsigned int n, int radix, const char *digit) {
    int result;

    if (n < radix) {
        putchar(digit[n]);
        return 1;
    }
    else {
        result = print_int_helper(n / radix, radix, digit);
        putchar(digit[n % radix]);
        return 1 + result;
    }
}
int print_int(int n, int radix, const char * digit) {
    if (radix < 2 || radix > 16) {
        toy_printf("Radix must be in [2..16]: Not %d\n", radix);
        exit(-1);
    }
    if (n > 0) {
        return print_int_helper(n, radix, digit);
    }
    if (n == 0) {
        putchar('0');
        return 1;
    }
    else {
        putchar('-');
        return 1 + print_int_helper(-n, radix, digit);
    }
    
}

int print_unsigned_int(unsigned int n, int radix, const char * digit) {
    if (radix < 2 || radix > 16) {
        toy_printf("Radix must be in [2..16]: Not %d\n", radix);
        exit(-1);
    }

    if (n > 0) {
        return print_unsigned_int_helper(n, radix, digit);
    }
    else {
        putchar('0');
        return 1;
    }
}
/* SUPPORTED:
 *   %b, %d, %o, %x, %X --
 *     integers in binary, decimal, octal, hex, and HEX
 *   %s -- strings
 */
void print_num_arr(int* arr, int size, int printRadix){
    putchar('{');
    for(int i=0; i<size; ++i){
        switch(printRadix){
            case 0: //decimal
                toy_printf("%d", *(arr+i));
                break;
            case 1: //unsigned
                toy_printf("%u", *(arr+i));
                break;
            case 2: //binary
                toy_printf("%b", *(arr+i));
                break;
            case 3: //octal
                toy_printf("%o", *(arr+i));
                break;
            case 4: //hexa
                toy_printf("%x", *(arr+i));
                break;
            case 5: //HEXA
                toy_printf("%X", *(arr+i));
                break;
        }
        if(i<size-1)
            toy_printf(", ");
        else
            putchar('}');
    }
}
int num_digits(int n){
    int res=1;
    if(n<0)
        n=-n;
    while (n>=10){
        n=n/10;
        ++res;
    }
    return res;
}
int toy_printf(char *fs, ...) {
    int chars_printed = 0;
    int int_value = 0;
    char *string_value;
    char char_value;
    va_list args;
    enum printf_state state;

    unsigned int unsigned_val = 0;
    int * int_arr;
    char * char_arr;
    char ** string_arr;
    int size;
    va_start(args, fs);
    char width[MAX_NUMBER_LENGTH];
    int widthIndex=1;
    int width_field;
    int deficit;

    state = st_printf_init;

    for (; *fs != '\0'; ++fs) {
        switch (state) {
            case st_printf_width_field:
                switch(*fs){
                    case 's':
                        string_value = va_arg(args, char *);

                        width_field = atoi(width);
                        if(width[0]!='-') {
                            deficit = width_field - strlen(string_value);
                        }
                        else {
                            deficit = width_field + strlen(string_value);
                        }
                        //left whitespace padding
                        while(deficit<0){
                            putchar(' ');
                            ++deficit;
                        }
                        //string print loop
                        while(*string_value){
                            chars_printed++;
                            putchar(*string_value);
                            string_value++;
                        }
                        //right whitespace padding
                        while(deficit>0){
                            putchar(' ');
                            if(deficit==1)
                                putchar('#');
                            --deficit;
                        }

                        state=st_printf_init;
                        width_field=0;
                        deficit=0;
                        break;
                    case 'd':
                        int_value = va_arg(args, int);
                        int num_of_digits=num_digits(int_value); //calculate num digits

                        width_field = atoi(width);
                        if(width[0]!='-') {
                            deficit = width_field - num_of_digits;
                        }
                        else {
                            deficit = width_field + num_of_digits;
                        }
                        if(width[0]=='0') { //turn mode: push 0's at left
                            deficit = -deficit;
                            if(int_value<0){ //if num<0 : fix - at start and change to positive num
                                putchar('-');
                                int_value=-int_value;
                                ++deficit; //reduce the deficit by 1
                            }
                        }
                        //left whitespace padding
                        while(deficit<0){
                            if(width[0]=='0') //if it's 0's mode: push 0, else push space
                                putchar('0');
                            else
                                putchar(' ');
                            ++deficit;
                        }
                        //print the decimal number
                        chars_printed += print_int(int_value, 10, digit);
                        //right whitespace padding
                        while(deficit>0){
                            if(width[0]=='0') //if it's 0's mode: break loop, else push spaces and #
                                break;
                            putchar(' ');
                            if(deficit==1)
                                putchar('#'); //eventually push #
                            --deficit;
                        }
                        
                        width_field=0;
                        deficit=0;
                        state=st_printf_init;
                        break;
                    default:
                        if(*fs>='0' && *fs<='9'){ //more number digits - grab them and reiterate
                            width[widthIndex]= *fs;
                            widthIndex++;
                        } else {
                            toy_printf("Unhandled format %%%c...\n", *fs);
                            exit(-1);
                        }
                }
                break;
                
            case st_printf_array:
                switch(*fs){ //for int print:0=d, u=1, b=2, o=3, x=4, X=5
                    case 'd':
                        int_arr = va_arg(args, int*);
                        size= va_arg(args, int);
                        print_num_arr(int_arr, size, 0);
                        state = st_printf_init;
                        break;
                    case 'u':
                        int_arr = va_arg(args, int*);
                        size= va_arg(args, int);
                        print_num_arr(int_arr, size, 1);
                        state = st_printf_init;
                        break;   
                    case 'b':
                        int_arr = va_arg(args, int*);
                        size= va_arg(args, int);
                        print_num_arr(int_arr, size, 2);
                        state = st_printf_init;
                        break;                     
                    case 'o':
                        int_arr = va_arg(args, int*);
                        size= va_arg(args, int);
                        print_num_arr(int_arr, size, 3);
                        state = st_printf_init;
                        break;                                                
                    case 'x':
                        int_arr = va_arg(args, int*);
                        size= va_arg(args, int);
                        print_num_arr(int_arr, size, 4);
                        state = st_printf_init;
                        break;
                    case 'X':
                        int_arr = va_arg(args, int*);
                        size= va_arg(args, int);
                        print_num_arr(int_arr, size, 5);
                        state = st_printf_init;
                        break;
                    case 's':
                        string_arr = va_arg(args, char**);
                        size= va_arg(args, int);
                        putchar('{');
                        for(int i=0; i<size; ++i){
                            toy_printf("%s", *(string_arr+i));
                            if(i<size-1)
                                toy_printf(", ");
                            else
                                putchar('}');
                        }
                        state = st_printf_init;
                        break;
                    case 'c':
                        char_arr = va_arg(args, char*);
                        size = va_arg(args, int);
                        putchar('{');
                        for(int i=0; i<size; ++i){
                            toy_printf("%c", *(char_arr+i));
                            if(i<size-1)
                                toy_printf(", ");
                            else
                                putchar('}');
                        }
                        state = st_printf_init;
                        break;
                    default:
                        toy_printf("Unhandled format %%%c...\n", *fs);
                        exit(-1);
                }
                break;
                
            case st_printf_init:
                switch (*fs) {
                    case '%':
                        state = st_printf_percent;
                        break;

                    default:
                        putchar(*fs);
                        ++chars_printed;
                }
                break;

            case st_printf_percent:
                switch (*fs) {
                    case '%':
                        putchar('%');
                        ++chars_printed;
                        state = st_printf_init;
                        break;

                    case 'd':
                        int_value = va_arg(args, int);
                        chars_printed += print_int(int_value, 10, digit);
                        state = st_printf_init;
                        break;

                    case 'u':
                        unsigned_val = va_arg(args, unsigned int);
                        chars_printed += print_unsigned_int(unsigned_val, 10, digit);
                        state = st_printf_init;
                        break;

                    case 'b':
                        unsigned_val = va_arg(args, unsigned int);
                        chars_printed += print_unsigned_int(unsigned_val, 2, digit);
                        state = st_printf_init;
                        break;

                    case 'o':
                        unsigned_val = va_arg(args, unsigned int);
                        chars_printed += print_unsigned_int(unsigned_val, 8, digit);
                        state = st_printf_init;
                        break;

                    case 'x':
                        unsigned_val = va_arg(args, unsigned int);
                        chars_printed += print_unsigned_int(unsigned_val, 16, digit);
                        state = st_printf_init;
                        break;

                    case 'X':
                        unsigned_val = va_arg(args, unsigned int);
                        chars_printed += print_unsigned_int(unsigned_val, 16, DIGIT);
                        state = st_printf_init;
                        break;

                    case 's':
                        string_value = va_arg(args, char *);
                        while(*string_value){
                            chars_printed++;
                            putchar(*string_value);
                            string_value++;
                        }
                        state = st_printf_init;
                        break;

                    case 'c':
                        char_value = (char)va_arg(args, int);
                        putchar(char_value);
                        ++chars_printed;
                        state = st_printf_init;
                        break;

                    case 'A':
                        state = st_printf_array;
                        break;

                    default:
                        if((*fs<='9' && *fs>='0') | (*fs=='-')){
                            width[0] = *fs;
                            state = st_printf_width_field;
                            break;
                        }
                        else{
                            toy_printf("Unhandled format %%%c...\n", *fs);
                            exit(-1);
                        }
                }
                break;

            default:
                toy_printf("toy_printf: Unknown state -- %d\n", (int) state);
                exit(-1);
          
        }
    }

    va_end(args);

    return chars_printed;
}
