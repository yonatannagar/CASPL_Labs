#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/** CONSTANTS */
#define MAX_NAME 100
#define newline printf("\n");

/** GLOBALS */
int fd = 0;
void* map_start;
struct stat st;
/** CODE */
void examine(){
    printf("Input ELF file name\n");
    char* name = calloc(1, MAX_NAME);
    scanf("%s", name);
    if(fd){
        munmap(map_start, st.st_size);
        close(fd);
        fd = 0;
    }
    int fd = open(name, O_RDONLY);
    if(fd < 0){
        printf("ERROR: open file failed\n");
        return;
    }
    // get fd length
    fstat(fd, &st);


    map_start = (Elf64_Ehdr *) mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    Elf64_Ehdr* ehdr = (Elf64_Ehdr*)map_start;
    if (ehdr == (Elf64_Ehdr *) (MAP_FAILED)) {
        fd = -1;
        printf("ERROR: mmap failed\n");
        close(fd);
        free(name);
        return;
    }
    if(ehdr->e_ident[0] != 127 || ehdr->e_ident[1] != 'E' || ehdr->e_ident[2] != 'L' || ehdr->e_ident[3] != 'F'){
        printf("ERROR: file is not an ELF file!\n");
        munmap(map_start, st.st_size);
        close(fd);
        free(name);
        return;
    }
    for (int i = 1; i < 4; ++i) {
        printf("%c", ehdr->e_ident[i]);
    }
    newline
    char* bits[] = {NULL, "32 bit", "64 bit"};
    char* endian[] = {NULL, "2's complement, little endian", "2's complement, big endian"};
    printf("Data:\t\t\t\t\t%s, %s\n", bits[ehdr->e_ident[4]], endian[ehdr->e_ident[5]]);
    printf("Entry point: \t\t\t\t0x%lx\n", ehdr->e_entry);
    printf("Section header table offset: \t\t%ld (bytes into file)\n", ehdr->e_shoff);
    printf("Number of section headers: \t\t%d\n", ehdr->e_shnum);
    printf("Size of section headers: \t\t%d (bytes)\n", ehdr->e_shentsize);
    printf("Program header table offset: \t\t%ld (bytes into file)\n", ehdr->e_phoff);
    printf("Number of program headers: \t\t%d\n", ehdr->e_phnum);
    printf("Size of program headers: \t\t%d (bytes)\n", ehdr->e_phentsize);


    free(name);
}
int simple_len(char* str){
    if(!str) return 0;
    int counter = 0;
    while(str[counter] != '\0')
        ++counter;
    return counter;
}
char* getSHType(int sh_type){
    char* sh_t[] = {    "NULL", "PROGBITS", "SYMTAB", "STRTAB",
                       "RELA", "HASH", "DYNAMIC", "NOTE",
                       "NOBITS", "REL", "SHLIB", "DYNSYM", "12PH",
                        "13PH", "INIT_ARRAY", "FINI_ARRAY", "PREINIT_ARRAY",
                        "GROUP", "SYMTAB_SHNDX", "NUM"}; //taken from elf.h
    if(sh_type >= 0 && sh_type < 20){
        return sh_t[sh_type];
    }
    else if(sh_type == 0x60000000) return "LOOS";
    else if(sh_type == 0x6FFFFFFF) return "HIOS";
    else if(sh_type == 0x70000000) return "LOPROC";
    else if(sh_type == 0x7FFFFFFF) return "HIPROC";
    else if(sh_type == 1879048182) return "GNU_HASH";
    else if(sh_type == 1879048190) return "VERNEED";
    else return "-NO-TYPE-";
}
void sections(){
    Elf64_Ehdr* ehdr = (Elf64_Ehdr*) map_start;
    Elf64_Shdr* shdr = (Elf64_Shdr*)(map_start + ehdr->e_shoff);
    printf("Index\tName\t\t\tAddress\t\tOffset\tSize\tType\n");
    int i, shnum = ehdr->e_shnum;
    Elf64_Shdr *sh_strtab = &shdr[ehdr->e_shstrndx];
    char* sh_strtab_abs = map_start + sh_strtab->sh_offset;
    for(i=0 ; i<shnum ; ++i){
        char* sh_name = sh_strtab_abs + shdr[i].sh_name;
        int len = simple_len(sh_name), padding = 23;
        printf("%d\t%s%*s%08lx\t\t0x%04lx\t%04ld\t%s\n",
               i, //index
               sh_name, //name
               (padding - len), "0x", //white padding to 24 (3 tabs)
               shdr[i].sh_addr, //address
               shdr[i].sh_offset, //offset
               shdr[i].sh_size, //size
               getSHType(shdr[i].sh_type)); //type
    }
}

void quit(){
    //Call frees here if needed!
    if(fd)
        close(fd);
    exit(0);
}
void menu(){
    printf("Choose action:\n");
    printf("1-Examine ELF File\n");
    printf("2-Print Section Names\n");
    printf("3-Quit\n");
}
int main(){
	void (*funcs[])() = {NULL,examine,sections,quit};

	int mod;
	while(1){
        menu();
		scanf("%d", &mod);
        if(mod == 0 || mod > 3) { //fail safe - illegal mod input
            printf("Illegal input, try again.\n");
            continue;
        }

        funcs[mod]();
	}

	return 0;
}