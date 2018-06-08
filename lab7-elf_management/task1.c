#include <malloc.h>
#include <unistd.h>
#include <memory.h>
#include <stdlib.h>


/** CONSTS */
#define MAX_FILENAME 100
#define READ_WRITE "r+"
#define READ "r"

/** CODE MACROES */
#define P_HEX_STR printf("Hexadecimal Representation:\n");
#define P_DEC_STR printf("\nDecimal Representation:\n");
#define FOR_LOOP for (i=0 ; i<n ; ++i)

/** GLOBALS */
char *filename = NULL;
int size = 1;

/** CODE */

void printArrs(unsigned char* str, int n){
    unsigned short* unShorts = (unsigned short*) str;
    unsigned int* unInts = (unsigned int*) str;
    int i;
    /*
    int i, print_mod = size * 2;
    
    void* arrays[5];
    arrays[1] = str;
    arrays[2] = unShorts;
    arrays[4] = unInts;

    printf("Hexadecimal Representation:\n");
    for( i=0 ; i<n ; ++i )
        printf("%0*x", print_mod, arrays[size][i]);
    printf("\nDecimal Representation:\n");
    for( i=0 ; i<n ; ++i )
        printf("%0*d", print_mod, arrays[size][i]);
    */

    switch(size){
        case 2:
            P_HEX_STR
            FOR_LOOP
                printf("%04x ", unShorts[i]);
            P_DEC_STR
            FOR_LOOP
                printf("%04d ", unShorts[i]);
            break;
        case 4:
            P_HEX_STR
            FOR_LOOP
                printf("%08x ",unInts[i]);
            P_DEC_STR
            FOR_LOOP
                printf("%08d ", unInts[i]);
            break;
        default: //case (size = 1):
            P_HEX_STR
            FOR_LOOP
                printf("%02x ", str[i]);
            P_DEC_STR
            FOR_LOOP
                printf("%02d ", str[i]);
            break;
    }
    printf("\n");
}

void setFileName(){
    //ONLY SUPPORTED 1 WORD NAME - no meta-chars
    printf("Enter file name:\n");
    scanf("%s", filename);
}

int legalSize(int s){
    if(s != 1 && s != 2 && s != 4)
        return 0;
    return 1;
} //in case we want to add support to 8/16/32 etc
void setUnitSize(){
    printf("Enter unit size:\n");
    int to_set;
    scanf("%d", &to_set);
    if(!legalSize(to_set)){
        printf("ERROR: invalid unit size given: %d\n", to_set);
        return;
    }
    size = to_set;
}

void quit(){
    //Call frees here if needed!
    if(filename)
        free(filename);
    exit(0);
}

void fileDisplay() {
    if(!filename){
        printf("ERROR: no filename specified\n");
        return;
    }

    FILE* f = fopen(filename, READ);
    if(!f){
        printf("ERROR: open file failed\n");
        return;
    }

    printf("Enter <location> <length>\n");

    int offset, len;
    scanf("%x %d", &offset, &len);

    unsigned char* read_buffer = calloc(len, size);

    if(fseek(f, offset, SEEK_SET) != 0){
        printf("ERROR: seek failed\n");
        fclose(f);
        free(read_buffer);
        return;
    }

    int n;

    if((n = fread(read_buffer, size, len, f))== -1 || n != len){
        printf("ERROR: read failed\n");
        free(read_buffer);
        return;
    }
    fclose(f);

    printArrs(read_buffer, n);

    free(read_buffer);
}

void fileModify(){
    if(!filename){
        printf("ERROR: no filename specified\n");
        return;
    }
    printf("Please enter <location> <val>\n");
    int offset;
    int val;
    scanf("%x %x", &offset, &val);

    FILE* f = fopen(filename, READ_WRITE);
    if(!f){
        printf("ERROR: open file failed\n");
        return;
    }
    if(fseek(f, offset, SEEK_SET) != 0){
        printf("ERROR: seek failed\n");
        fclose(f);
        return;
    }

    unsigned char byte;
    unsigned short byte2;
    unsigned int byte4;
    switch(size){
        case 2:
            byte2 = (unsigned short) val;
            fwrite(&byte2, size , 1, f);
            break;
        case 4:
            byte4 = (unsigned int) val;
            fwrite(&byte4, size , 1, f);
            break;
        default:
            byte = (unsigned char) val;
            fwrite(&byte, size , 1, f);
            break;
    }

    fclose(f);
}

void copyFromFile(){
    printf("Please enter <src_file> <src_offset> <dst_offset> <length>\n");
    char* src_name = calloc(MAX_FILENAME, 1);
    int src_offset, dst_offset, length;
    scanf("%s %x %x %d", src_name, &src_offset, &dst_offset, &length);

    if(!filename || !src_name){
        printf("ERROR: src or dst doesn't exist\n");
        if(src_name)
            free(src_name);
        return;
    }

    /** OPENING FILES */
    FILE* src = fopen(src_name, READ);
    FILE* dst = fopen(filename, READ_WRITE);
    if(!src || !dst) {
        printf("ERROR: opening files failed\n");
        free(src_name);
        if(src) fclose(src);
        if(dst) fclose(dst);
        return;
    }

    /** SEEKING */
    if(fseek(src, src_offset, SEEK_SET) != 0){
        printf("ERROR: src seek failed\n");
        fclose(src);
        fclose(dst);
        return;
    }
    if(fseek(dst, dst_offset, SEEK_SET) != 0){
        printf("ERROR: dst seek failed\n");
        fclose(src);
        fclose(dst);
        return;
    }

    /** READ -> BUFFER -> WRITE */
    unsigned char* buffer = calloc(1, length);

    int n, m;
    if((n = (int) fread(buffer, 1, (size_t) length, src)) == -1 || n != length){
        printf("ERROR: read failed\n");
        free(buffer);
        free(src_name);
        fclose(src);
        fclose(dst);
        return;
    }

    if((m = (int) fwrite(buffer, 1, (size_t) n, dst)) == -1 || m != n){
        printf("ERROR: write failed\n");
        free(buffer);
        free(src_name);
        fclose(src);
        fclose(dst);
        return;
    }
    /** Output response */
    printf("Copied %d bytes FROM %s at %x TO %s at %x\n", m, src_name, src_offset, filename, dst_offset);
    free(buffer);
    free(src_name);
    fclose(src);
    fclose(dst);

}

int main(){
    // init function array
    void (*funcs[])() = { NULL,
                          setFileName,
                          setUnitSize,
                          fileDisplay,
                          fileModify,
                          copyFromFile,
                          quit};

    int mod;
    filename = malloc(MAX_FILENAME);

    while(1) {
        printf("Choose action:\n");
        printf("1-Set File Name\n");
        printf("2-Set Unit Size\n");
        printf("3-File Display\n");
        printf("4-File Modify\n");
        printf("5-Copy From File\n");
        printf("6-Quit\n");
        scanf("%d", &mod);

        if(mod == 0 || mod > 6) //fail safe - illegal mod input
            continue;

        funcs[mod]();
    }
    return 0;
}
