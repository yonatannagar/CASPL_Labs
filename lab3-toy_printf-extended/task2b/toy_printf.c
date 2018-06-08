#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>
#include <stdbool.h>

/* the states in the printf state-machine */
enum printf_state {
    st_printf_init,
    st_printf_percent,
    st_printf_array,
    st_printf_padding
};

#define MAX_NUMBER_LENGTH 64

int toy_printf(char *fs, ...);

const char *digit = "0123456789abcdef";
const char *DIGIT = "0123456789ABCDEF";

typedef struct {
    char* fs;
    int int_value;
    char *string_value;
    char char_value;
    unsigned int unsigned_val;
    int * int_arr;
    char * char_arr;
    char ** string_arr;
    int size;
    char width[MAX_NUMBER_LENGTH];
    int widthIndex;
    int width_field;
}state_args;
typedef struct{
    int printed_chars;
    enum printf_state new_state;
}state_result;


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
int print_num_arr(int* arr, int size, int printRadix){
    char format[3] = {'%', "duboxX"[printRadix], '\0'};

    int counter = toy_printf("{");
    for(int i=0; i<size; ++i){
        counter += toy_printf(format, *(arr+i));
        if(i<size-1)
            counter += toy_printf(", ");
        else
            counter += toy_printf("}");
    }
    return counter;
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
/*
state_result num_print_handler(va_list args, state_args* state){
    state_result res;
    res.new_state = st_printf_init;


    return res;
}
*/
state_result init_state_handler(va_list args, state_args* state){
    state_result result;
    switch (*state->fs) {
        case '%':
            result.printed_chars = 0;
            result.new_state = st_printf_percent;
            break;
        default:
            putchar(*state->fs);
            result.printed_chars=1;
            result.new_state = st_printf_init;
    }
    return result;
}
state_result array_state_handler(va_list args, state_args* state) {
    state_result res;
    res.printed_chars = 0;

    switch(*state->fs){ //for int print:0=d, u=1, b=2, o=3, x=4, X=5
        case 'd':
            state->int_arr = va_arg(args, int*);
            state->size= va_arg(args, int);
            res.printed_chars += print_num_arr(state->int_arr, state->size, 0);
            res.new_state = st_printf_init;
            return res;

        case 'u':
            state->int_arr = va_arg(args, int*);
            state->size= va_arg(args, int);
            res.printed_chars += print_num_arr(state->int_arr, state->size, 1);
            res.new_state = st_printf_init;
            return res;

        case 'b':
            state->int_arr = va_arg(args, int*);
            state->size= va_arg(args, int);
            res.printed_chars += print_num_arr(state->int_arr, state->size, 2);
            res.new_state = st_printf_init;
            return res;

        case 'o':
            state->int_arr = va_arg(args, int*);
            state->size= va_arg(args, int);
            res.printed_chars += print_num_arr(state->int_arr, state->size, 3);
            res.new_state = st_printf_init;
            return res;

        case 'x':
            state->int_arr = va_arg(args, int*);
            state->size= va_arg(args, int);
            res.printed_chars += print_num_arr(state->int_arr, state->size, 4);
            res.new_state = st_printf_init;
            return res;

        case 'X':
            state->int_arr = va_arg(args, int*);
            state->size= va_arg(args, int);
            res.printed_chars += print_num_arr(state->int_arr, state->size, 5);
            res.new_state = st_printf_init;
            return res;

        case 's':
            state->string_arr = va_arg(args, char**);
            state->size = va_arg(args, int);
            int j = state->size;

            res.printed_chars += toy_printf("{");
            for(int i=0; i<j; ++i){
                res.printed_chars += toy_printf("%s", state->string_arr[i]);
                if(i<j-1)
                    res.printed_chars += toy_printf(", ");
                else
                    res.printed_chars += toy_printf("}");

            }
            res.new_state =  st_printf_init;
            return res;

        case 'c':
            state->char_arr = va_arg(args, char*);
            state->size = va_arg(args, int);
            int k = state->size;

            res.printed_chars += toy_printf("{");
            for(int i=0; i<k ; ++i){
                res.printed_chars += toy_printf("%c", state->char_arr[i]);
                if(i<k-1)
                    res.printed_chars += toy_printf(", ");
                else
                    res.printed_chars += toy_printf("}");
            }

            res.new_state = st_printf_init;
    }
    res.new_state = st_printf_init;
    return res;
}
state_result padding_state_handler(va_list args, state_args* state){
    state_result res;
    res.printed_chars = 0;

    int pad;
    bool zero_padding=false;
    switch(*state->fs){
        case 's':
            state->string_value = va_arg(args, char *);

            state->width_field = atoi(state->width);
            if(state->width[0]!='-')
                pad = (int) (state->width_field - strlen(state->string_value));
            else
                pad = (int) (state->width_field + strlen(state->string_value));

            //left whitespace padding
            while(pad<0){
                res.printed_chars += toy_printf(" ");
                ++pad;
            }
            //string print
            res.printed_chars += toy_printf("%s", state->string_value);
            //right whitespace padding
            while(pad>0) {
                res.printed_chars += toy_printf(" ");
                if (pad == 1)
                    res.printed_chars += toy_printf("#");
                --pad;
            }
            res.new_state = st_printf_init;
            break;

        case 'd':
            state->int_value = va_arg(args, int);
            int num_of_digits=num_digits(state->int_value); //calculate num digits

            state->width_field = atoi(state->width);
            if(state->width[0]!='-')
                pad = state->width_field - num_of_digits;
            else
                pad = state->width_field + num_of_digits;
            zero_padding = (state->width[0]=='0');

            if(zero_padding) { //turn mode: push 0's at left
                pad = -pad;
                if(pad<0){ //if num<0 : fix - at start and change to positive num
                    res.printed_chars += toy_printf("-");
                    state->int_value = -1 * (state->int_value);
                    ++pad; //reduce the deficit by 1
                }
            }
            //left whitespace padding
            while(pad<0){
                if(zero_padding) //if it's 0's mode: push 0, else push space
                    res.printed_chars += toy_printf("0");
                else
                    res.printed_chars += toy_printf(" ");
                ++pad;
            }//print the decimal number
            res.printed_chars += print_int(state->int_value, 10, digit);
            //right whitespace padding
            while(pad>0){
                if(state->width[0]=='0') //if it's 0's mode: break loop, else push spaces and #
                    break;
                res.printed_chars += toy_printf(" ");
                if(pad==1)
                    res.printed_chars += toy_printf("#");; //eventually push #
                --pad;
            }

            res.new_state = st_printf_init;
            break;

        default:
            if(*state->fs>='0' && *state->fs<='9'){ //more number digits - grab them and reiterate
                state->widthIndex++;
                state->width[state->widthIndex]= *state->fs;
            } else {
                toy_printf("Unhandled format %%%c...\n", *state->fs);
            }
            res.new_state = st_printf_padding;
    }

    return res;
}
state_result percent_state_handler(va_list args, state_args *state){
    state_result res;
    res.printed_chars = 0;


    switch (*state->fs) {
        case '%':
            putchar('%');
            res.new_state=st_printf_init;
            ++res.printed_chars;
            return res;

        case 'd':
            //state->int_value = va_arg(args, int);
            res.printed_chars += print_int(va_arg(args, int), 10, digit);
            res.new_state=st_printf_init;
            return res;

        case 'u':
            //state->unsigned_val = va_arg(args, unsigned int);
            res.printed_chars += print_unsigned_int(va_arg(args, unsigned int), 10, digit);
            res.new_state=st_printf_init;
            return res;

        case 'b':
            res.printed_chars += print_unsigned_int(va_arg(args, unsigned int), 2, digit);
            res.new_state=st_printf_init;
            return res;

        case 'o':
            res.printed_chars += print_unsigned_int(va_arg(args, unsigned int), 8, digit);
            res.new_state=st_printf_init;
            return res;

        case 'x':
            res.printed_chars += print_unsigned_int(va_arg(args, unsigned int), 16, digit);
            res.new_state=st_printf_init;
            return res;

        case 'X':
            res.printed_chars += print_unsigned_int(va_arg(args, unsigned int), 16, DIGIT);
            res.new_state=st_printf_init;
            return res;

        case 's':
            state->string_value = va_arg(args, char *);
            while(*state->string_value){
                ++res.printed_chars;
                putchar(*state->string_value);
                state->string_value++;
            }
            res.new_state=st_printf_init;
            return res;

        case 'c':
            state->char_value = (char)va_arg(args, int);
            putchar(state->char_value);
            ++res.printed_chars;
            res.new_state=st_printf_init;
            return res;

        case 'A':
            res.new_state = st_printf_array;
            return res;
        default:
            if((*state->fs<='9' && *state->fs>='0') | (*state->fs=='-')){
                state->width[0] = *state->fs;
                res.new_state=st_printf_padding;
                return res;
            }
            else{
                toy_printf("Unhandled format %%%c...\n", *state->fs);
            }
    }
    res.new_state=st_printf_init;
    return res;
}

int toy_printf(char *fs, ...) {
    int chars_printed = 0; //return value accumulator

    //Handlers array
    state_result (*handlers[4])(va_list, state_args*) = {init_state_handler, percent_state_handler, 
                                                                array_state_handler, padding_state_handler};                                                       
    
    va_list args;
    va_start(args, fs);
    //Handler args init
    state_args* state_args = malloc(sizeof(state_args));
    state_args->fs = fs;

    //Handlers result init
    state_result result;
    result.printed_chars=0;
    result.new_state = st_printf_init;



    //iteration
    for (; *state_args->fs != '\0'; ++(state_args->fs)) {
        result = handlers[result.new_state](args, state_args);
        chars_printed += result.printed_chars;
    }

    va_end(args);

    return chars_printed;
}