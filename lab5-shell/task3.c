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
#define ERR_FOPEN "open failed: "
#define ERR_FORK "fork failed: "
#define newline "\n"
#define q "quit\n"
#define ERR_EXECVP "execvp failed: "


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

void exec_parent(cmd_line* line, int pid, int pid2){
    if(line->blocking == 1) {
        waitpid(pid, NULL, 0);
        if (line->next){
            waitpid(pid2, NULL, 0);
        }
    }
}
void execute(cmd_line *line) {
    int _pipe[2];
    int two_lines = 0;
    if(line->next){
        pipe(_pipe);
        two_lines = 1;
    }

    int pid = fork();

    switch(pid){
        case 0: //is 1st child
            if(two_lines){
                dup2(_pipe[1], fileno(stdout));
                close(_pipe[1]);
            }
            exec_child(line);
            break;
        case -1: //fork error
            perror(ERR_FORK);
            _exit(errno);
        default:
            if(two_lines){
                close(_pipe[1]);
            }else //only 1 line
                exec_parent(line, pid, 0/** placeholder 0 */);
    }

    if (two_lines){
        int pid2 = fork();

        switch(pid2){
            case 0: //is 2nd child
                dup2(_pipe[0], fileno(stdin));
                close(_pipe[0]);
                exec_child(line->next);
                break;
            case -1: //fork error
                perror(ERR_FORK);
                _exit(errno);
            default:
                close(_pipe[0]);
                exec_parent(line, pid, pid2);
        }
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
