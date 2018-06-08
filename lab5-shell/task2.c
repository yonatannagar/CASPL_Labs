#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <wait.h>


/** STRINGS AND FORMATS **/

#define ERR_FORK "fork failed: "
#define ERR_EXECVP "execvp failed: "


int main(int argc, char *argv[]){
    int _pipe[2]; //[0] = read end, [1] = write end
    pipe(_pipe);
    // child 1 creation and running
    int pid = fork();
    switch(pid){
        case 0:
            fclose(stdout);           
            dup(_pipe[1]);

            close(_pipe[1]);        
            char* cmd[] = {"ls", "-l", NULL};

            if (execvp(cmd[0], cmd) == -1){
                perror(ERR_EXECVP);
                _exit(errno);
            }
            break;

        case -1:
            perror(ERR_FORK);
            _exit(errno);
        default:
            close(_pipe[1]);                                /** STEP 4 **/
            NULL;

    }

    // child 2 creation and running
    int pid2 = fork();
    switch(pid2){
        case 0:
            fclose(stdin);            
            dup(_pipe[0]);

            close(_pipe[0]);          

            char* cmd[] = {"tail", "-n", "2", NULL};

            if (execvp(cmd[0], cmd) == -1){
                perror(ERR_EXECVP);
                _exit(errno);
            }
            break;


        case -1:
            perror(ERR_FORK);
            _exit(errno);
        default: //parent waiting for children 1, 2
            close(_pipe[0]); //parent closing read end      /** STEP 7 **/  
            waitpid(pid, NULL, 0);                          /** STEP 8a **/
            waitpid(pid2, NULL, 0);                         /** STEP 8b **/
    }
    return 0;
}