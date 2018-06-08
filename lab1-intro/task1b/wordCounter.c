#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>

void wordCounter(char* str){
    int i=0,res=0;
    while(1){
        bool flag = false;
        if(str[i]==0) {
            break;
        }
        while(str[i]!=0 && str[i] <= ' ') //White blanks at start
            ++i;
        while(str[i]!=0 && str[i] > ' '){  //located a word proceed to it's end
            ++i;
            flag=true;
        }
        if(flag)
            ++res;
    }
    printf("%d\n", res);
}
void charCounter(char* str){
    int i=0, res=0;
    while(str[i]!=0){
        if(str[i]>' ')
            ++res;
        i++;
    }
    printf("%d\n", res);
}
void longestWordCharCounter(char* str){
    int i=0, curr=0, res=0;
    while(str[i]!=0){
        if(str[i]>' ')
            ++curr;
        else {
            if (curr >= res)
                res = curr;
            curr = 0;
        }
        i++;
    }
    printf("%d\n", res);
}

int main(int argc, char** argv) {
    char str_buf [MAX_INPUT];
    int i, c=0;
    bool wF=false, cF=false, lF=false;

    for (i=0; i<argc; ++i){
        if(strcmp(argv[i], "-w")==0)
            wF=true;
        if(strcmp(argv[i], "-c")==0)
            cF=true;
        if(strcmp(argv[i], "-l")==0)
            lF=true;
    }

    while(1){
        if(feof(stdin))
            break;
        str_buf[c] = fgetc(stdin);
        ++c;
    }

    str_buf[c]=0;
    printf("\n");
    
    if(!wF && !cF && !lF)
        wordCounter(str_buf);
    else{
        if(wF)
            wordCounter(str_buf);
        if(cF)
            charCounter(str_buf);     
        if(lF) 
            longestWordCharCounter(str_buf);
    }

    return 0;
}