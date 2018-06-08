#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <memory.h>
#include <wait.h>
#include "line_parser.h"

#define MAX_CMD 2048

/** STRINGS AND FORMATS **/
#define READ "r"
#define WRITE "w"
#define ERR_FOPEN "open failed"
#define ERR_FORK "fork failed"
#define newline "\n"
#define q "quit\n"
#define ERR_EXECVP "execvp failed"


void exec_child(cmd_line* line){
    FILE* io[2]; 

    if(line->input_redirect){
        io[0] = fopen(line->input_redirect, READ);
        if(!io[0]) {
            perror(ERR_FOPEN);
            _exit(errno);
        }
        else
            dup2(fileno(io[0]), fileno(stdin));
    }

    if(line->output_redirect){
        io[1] = fopen(line->output_redirect, WRITE);
        if(!io[1]) {
            perror(ERR_FOPEN);
            _exit(errno);
        }
        else
            dup2(fileno(io[1]), fileno(stdout));
    }

    if(execvp(line->arguments[0], line->arguments) == -1){
        perror(ERR_EXECVP);
        _exit(errno);
    }
    if(io[0]) fclose(io[0]);
    if(io[1]) fclose(io[1]);
}

void exec_parent(cmd_line* line, int pid){
    if(line->blocking == 1)
        waitpid(pid, NULL, 0);
}

void execute(cmd_line *line) {
    int pid = fork();

    switch(pid){
        case 0: //is child
            exec_child(line);
            break;
        case -1: //fork error
            perror(ERR_FORK);
            _exit(errno);
        default:
            exec_parent(line, pid);
    }
}

int main() {
    char buf[MAX_CMD];
    cmd_line* cmd;
    char cwd[PATH_MAX];

    while(1){
        //print cwd
        getcwd(cwd, PATH_MAX);
        fputs(cwd, stdout);
        fputs("$:", stdout);

        fgets(buf, MAX_CMD, stdin);
        if(strcmp(buf, q) == 0){
            break;
        }
        else if(strcmp(buf, newline) != 0) {
            cmd = parse_cmd_lines(buf);

            execute(cmd);
            free_cmd_lines(cmd);
        }
    }

    return 0;
}
