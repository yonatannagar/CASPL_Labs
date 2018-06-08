#include "lab4_util.h"

#define SYS_WRITE 1
#define SYS_OPEN 2
#define SYS_LSEEK 8
#define SYS_CLOSE 3
#define SYS_GETDENTS 78

#define SYS_EXIT 60
#define O_RDONLY 0x000
#define O_RDWR 0x002
#define BUF_SIZE 2<<10
#define premissions	0777
#define STDOUT 1
#define newline "\n"

extern int system_call();

char* input_name;
int name_mod = 0;

char* input_cmd;
int cmd_mod = 0;
int file_exists = 0;

struct linux_dirent {
	long  			d_ino;     /* Inode number */
	long  			d_off;     /* Offset to next linux_dirent */
	unsigned short 	d_reclen;  /* Length of this linux_dirent */
	char           	d_name[];  /* Filename (null-terminated) */
};



void crush(int n)
{
	if(n<0)
		system_call(SYS_EXIT, 0x55);
}

int open(char* p, int o_status, int chmod){
	int fd = system_call(SYS_OPEN, p, o_status, chmod);
	crush(fd);

	return fd;
}
int getdents(int fd, char* buf, int size){
	int nread = system_call(SYS_GETDENTS, fd, buf, size);
	crush(nread);
	return nread;
}
int write(int fd, char* to_write){
	int write = system_call(SYS_WRITE, fd, to_write, simple_strlen(to_write));
	crush(write);
	return write;
}
void close(int fd){
	int fin = system_call(SYS_CLOSE, fd);
	crush(fin);
}

void dir_p(char* p)
{
	int fd = open(p, O_RDONLY, premissions);
	
	int nread;
    char buf[BUF_SIZE];
    struct linux_dirent *d;
    int bpos;
    char d_type;


	for ( ; ; ) {
		nread = getdents(fd, buf, BUF_SIZE);
		if (nread == 0){
			break;
		}
		for (bpos = 0; bpos < nread;) {
			d = (struct linux_dirent *) (buf + bpos); /* d = pick a file */
			d_type = *(buf + bpos + d->d_reclen - 1); /* 4 is dir */

			bpos += d->d_reclen;
			char* name = d->d_name;

			if((simple_strcmp(name, "..") == 0 || (simple_strcmp(name, ".") == 0))){
				continue;
			}
			else{
				/*fix new path: old_path... /name */
				int len_p = simple_strlen(p);
				int len_name = simple_strlen(name);

				char full_path[len_p + 1 + len_name];
				int j;
				for(j = 0; j<len_p ; ++j){
					full_path[j] = p[j];
				}
				full_path[len_p] = '/';
				for (j = 0 ; j < len_name ; ++j){
					full_path[j + len_p + 1] = name[j];
				}
				full_path[len_p+len_name+1] = 0; 

				/* fixed full path, 0 @ end */

				if(name_mod == 0 && cmd_mod == 0){
					write(STDOUT, full_path);
					write(STDOUT, newline);
    			}
    			else if(name_mod == 1){ 
					/*name_mod == 1*/
					if(simple_strcmp(input_name, name) == 0){ 
						/*name = asked for name */
						write(STDOUT, full_path);
						write(STDOUT, newline);						
    				}
    			}else /*cmd_mod == 1*/{
    				if(simple_strcmp(name, input_name) == 0){
    					int len_full_path = simple_strlen(full_path);
    					int len_cmd = simple_strlen(input_cmd);

    					char full_and_cmd[len_full_path + len_cmd + 1];
    					int i;
    					for(i = 0 ; i < len_cmd ; ++i){
    						full_and_cmd[i] = input_cmd[i];
    					}
    					full_and_cmd[len_cmd] = ' ';
    					for(i = 0 ; i < len_full_path ; ++i){
    						full_and_cmd[len_cmd + 1 + i] = full_path[i];
    					}
    					full_and_cmd[len_cmd + len_full_path + 1] = 0;

    					/* string fixed */
    					simple_system(full_and_cmd);
    					file_exists = 1;
    					
    				}
    			}

                if(d_type == 4){
                	dir_p(full_path);
                }
			}
		}


	}
	close(fd);
}

int main (int argc , char* argv[], char* envp[])
{
	int i;
	for ( i = 0 ; i < argc ; ++i){
		if (simple_strcmp(argv[i], "-n") == 0){
			name_mod = 1;
			input_name = argv[++i];
		}
		if(simple_strcmp(argv[i], "-e") == 0){
			cmd_mod = 1;
			input_name = argv[++i];
			input_cmd = argv[++i];
		}
	}

	char* p = ".";
	dir_p(p);


	if(cmd_mod == 1 && file_exists == 0){
		char* msg1 = "The file ";
		char* msg2 = " does not exist\n";

		write(STDOUT, msg1);
		write(STDOUT, input_name);
		write(STDOUT, msg2);
	}	

	return 0;
}
