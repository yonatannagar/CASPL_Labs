#include "lab4_util.h"

#define SYS_WRITE 1
#define SYS_OPEN 2
#define SYS_LSEEK 8
#define SYS_CLOSE 3

#define SYS_EXIT 60
#define O_RDWR 0x002

extern int system_call();
void crush(int n)
{
	if(n<0)
		system_call(SYS_EXIT, 0x55);
}
char* fix_length(char* str)
{
	int str_len = simple_strlen(str);
	int i;
	if(str_len < 5){
		for(i=str_len ; i<5 ; ++i){
			str[i] = ' ';
		}
		str[5] = '\n';
	}
	return str;
}
void fix_file(char* file_name, char* input_str){
	int fd = system_call(SYS_OPEN, file_name, O_RDWR, 0777);
	crush(fd);

	int file = system_call(SYS_LSEEK, fd, 0x1015, 0); 
	crush(file);

	char* new_str = fix_length(input_str);

	int w = system_call(SYS_WRITE, fd, new_str, simple_strlen(new_str));
	crush(w);

	int fin = system_call(SYS_CLOSE, fd);
	crush(fin);
	
}
int main (int argc , char* argv[], char* envp[])
{	
	int i;

	char* file_name;
	char* input_str;
	
	for (i = 0 ; i < argc ; ++i){
		if(i==1){ 
			file_name = argv[i];
		}
		if(i==2){
			input_str = argv[i];
		}
	}
	fix_file(file_name, input_str);


	return 0;
}
