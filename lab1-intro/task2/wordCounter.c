#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>

void count(char c, int counters[]){
    if(counters[5]==0 && c > ' '){
        counters[5]=1;
        counters[0]++;
    }
    if(c=='\n')
        counters[3]++;
    if(c <= ' '){
        counters[5]=0;
        if(counters[4]>counters[2])
            counters[2]=counters[4];
        
        counters[4]=0;   
    }else{
        counters[1]++;
        counters[4]++;
    }

}
int main(int argc, char** argv) {
    int i;
    bool useFile=false, wF=false, cF=false, lF=false, nF=false;
    int nameIndex;
    int counters[]={0,0,0,0,0,0};//[word, char, longest, \n, currWord, wordFlag]
    for (i=0; i<argc; ++i){
        if(strcmp(argv[i], "-w")==0)
            wF=true;
        if(strcmp(argv[i], "-c")==0)
            cF=true;
        if(strcmp(argv[i], "-l")==0)
            lF=true;
        if(strcmp(argv[i], "-n")==0)
            nF=true;
        if(strcmp(argv[i], "-i")==0){
            useFile = true;
            nameIndex = i+1;
        }
    }
    char c;
    FILE *f;
    if(useFile){
        f = fopen(argv[nameIndex], "r");
        while((c=fgetc(f))!=EOF)
            count(c, counters);
        
    }
    /*insert NULL last*/
    else{
        while((c=fgetc(stdin))!=EOF){
               count(c, counters);
        }            
    } 
    /* str_buf ready for work */
    printf("\n");

    if(!wF && !cF && !lF && !nF)
        printf("%d\n", counters[0]);
    else{
        if(wF)
            printf("%d\n", counters[0]);
        if(cF)
            printf("%d\n", counters[1]);
        if(lF) 
            printf("%d\n", counters[2]);
        if(nF)
            printf("%d\n", counters[3]);
    }
    if(useFile)
        fclose(f);
    
    return 0;
}