#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "line_parser.h"
#include "common.h"


int debug = FALSE;
cmd_line *cmd;
client_state state;
char buff[MAX_LEN];


void debug_print(){
    if(debug){
        if(state.sock_fd == -1) {
            fprintf(stderr, "%s|Log %s\n", cmd->arguments[1], buff);
        }else{
            fprintf(stderr, "%s|Log %s\n", state.server_addr, buff);
        }
    }
}
void init_state(){
    state.server_addr = "nil";
    state.conn_state = IDLE;
    state.client_id = NULL;
    state.sock_fd = -1;
}
void refresh_buf(){
    int i=0;
    do{
        buff[i] = 0;
        ++i;
    }while(buff[i] != 0);
}
int isQuit(char* str){
    if(simple_cmp(str, "quit", 0))
        return 1;
    return 0;
}

int send_msg(int fd, char* msg){
    int w = (int) write(fd, msg, strlen(msg));
    fsync(fd);
    if(w < 0){
        printf("Client @ Write failed\n");
        return -1;
    }
    return w;
}
int recv_msg(int fd, char* in_buff){
    //memset(in_buff, '\0', MAX_LEN);
    int r = (int) recv(fd, in_buff, MAX_LEN, 0);
    if(r<0){
        printf("Client @ Read failed\n");
        return -1;
    }
    if(strncmp(buff, "nok", 3) == 0){
        return -1;
    }
    debug_print();
    return r;
}

int handle_get(){
    if(state.conn_state != CONNECTED)
        return -2;
    int file_size, itr, wait_cycle = 10;
    char filename[MAX_LEN], in_buff[FILE_BUFF_SIZE];
    memset(filename, '\0', MAX_LEN);
    memset(in_buff, '\0', FILE_BUFF_SIZE);
    strcpy(filename, "get ");
    strcat(filename, cmd->arguments[1]);

    printf("Trying to %s\n", filename);
    if(send_msg(state.sock_fd, filename) < 0)
        return -1;
    if(recv_msg(state.sock_fd, in_buff) < 0 ||
       strncmp(in_buff, "nok file", strlen("nok file")) == 0)
        return -1;

    sscanf(in_buff, "ok %d", &file_size);
    itr = file_size / FILE_BUFF_SIZE; // amount of data chunks to be received
    printf("Client @ file:%s, size:%d bytes\n", cmd->arguments[1], file_size);
    //printf("%s\n", in_buff);
    state.conn_state = DOWNLOADING;

    strcat(filename, ".tmp");
    FILE* file = fopen(filename+4, WRITE_EDIT_CREATE);
    while(itr > -1){
        memset(in_buff, '\0', FILE_BUFF_SIZE);
        printf("waiting for %d chunk\n", (file_size / FILE_BUFF_SIZE) - itr + 1);
        fsync(state.sock_fd);

        if(recv_msg(state.sock_fd, in_buff) < 0){
            perror("Client @ Error in receiving file!\n");
            fclose(file);
            if(remove(filename+4) != 0){
                printf("Client @ removing tmp file failed\n");
            }
            FREE(state.client_id);
            FREE(state.server_addr);
            init_state();
            return 0;

        }
        fsync(state.sock_fd);
        write(file->_fileno, in_buff, strlen(in_buff));
        printf("in_buff content size: %d\n", (int) strlen(in_buff));
        itr--;
    }
    printf("DONE!\n");
    // now send done
    if(send_msg(state.sock_fd, "done") < 0)
        return -1;
    //wait cycle, check ? yes=>( ok ? break : return -1) : no=>wait 1s
    while(wait_cycle > 0){
        if(recv_msg(state.sock_fd, in_buff) < 0 || strncmp("nok", in_buff, 3) == 0) {
            char err[MAX_LEN];
            memset(err, '\0', MAX_LEN);
            strcpy(err, "Server Error: ");
            strcat(err, in_buff+3);
            perror(err);
            fclose(file);
            if(remove(filename+4) != 0){
                printf("Client @ removing tmp file failed");
            }
            FREE(state.client_id);
            FREE(state.server_addr);
            init_state();
            return -1;
        }
        if(strncmp("ok", in_buff, 2) == 0)
            break;
        wait_cycle--;
        sleep(ONESEC);
    }
    if(wait_cycle == 0){
        //delete tmp file
        //print error msg
        perror("Client @ Error in receiving file!\n");
        fclose(file);
        if(remove(filename+4) != 0){
            printf("Client @ removing tmp file failed\n");
        }

        FREE(state.client_id);
        FREE(state.server_addr);
        init_state();
        return -1;
    }

    //rename file.tmp ==> file
    //state <- connected
    if(rename(filename, cmd->arguments[1]) < 0)
        printf("Client @ can't rename %s to %s\n", filename, cmd->arguments[1]);
    state.conn_state = CONNECTED;
    fclose(file);
    printf("Client @ Successfully created \"%s\"!\n", cmd->arguments[1]);
    return 0;
}

int exec(cmd_line* cmd) {
    int fd;
    struct sockaddr_in server;
    char in_buff[MAX_LEN];

    if(simple_cmp(cmd->arguments[0], "conn", 0)){
        if(state.conn_state != IDLE){
            printf("Client @ status is not IDLE!\n");
            return -2;
        }
        if((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // SOCK_STREAM = TCP socket
            perror("ERROR: socket creation failed\n");
            exit(-1);
        }
        server.sin_addr.s_addr = inet_addr(cmd->arguments[1]);
        server.sin_family = AF_INET;
        server.sin_port = htons(MAGIC_PORT);

        if(connect(fd, (struct sockaddr*)&server, sizeof(server)) < 0){
            perror("Client @ connection failed \n");
            return -1;
        }

        if(send_msg(fd, "conn") <= 0){
            printf("Client @ Handshake failed, sending hello failed\n");
            close(fd);
            return -1;
        }
        // Waiting for ACK in the form of hello <ID string>
        state.conn_state = CONNECTING;
        if(recv_msg(fd, in_buff) == -1){
            printf("Client @ Handshake failed, received %s\n", in_buff);
            return -1;
        }
        if(strncmp(in_buff, "hello ", strlen("hello ")) != 0){
            printf("Client @ Expected \"hello \" but received \"%s\"\n", in_buff);
            return -1;
        }

        // Handshake completed successfully - fill in "fields"
        // 6 = strlen("hello ")
        state.client_id = malloc(strlen(in_buff) - 6 + 1);
        strcpy(state.client_id, in_buff + 6); //Fill in client ID

        state.conn_state = CONNECTED;
        state.sock_fd = fd;

        state.server_addr = malloc(strlen(cmd->arguments[1]) + 1);
        strcpy(state.server_addr, cmd->arguments[1]);

        return 0;

    }
    else if(simple_cmp(cmd->arguments[0], "bye", 0)){
        if(state.conn_state == CONNECTED){
            if(send_msg(state.sock_fd, "bye") == -1){
                return -1;
            }

            FREE(state.client_id);
            FREE(state.server_addr);
            init_state();
            return 0;
        }else{
            printf("Client @ You can't disconnect when you're not connected\n" );
            return -2;
        }

    }// TO ADD MORE COMMANDS WRITE ELSE IF(STRCMP(X, Y) == 0)...
    else if(strcmp(cmd->arguments[0], "ls") == 0){
        if(state.conn_state != CONNECTED){
            printf("Client @ You are not connected!\n");
            return -2;
        }
        if(send_msg(state.sock_fd, "ls") == -1){
            return -1;
        }
        memset(in_buff, '\0', MAX_LEN);
        if(recv_msg(state.sock_fd, in_buff) == -1){
            return -1;
        }

        if(strncmp("ok ", in_buff, 3) == 0){
            // DEAL WITH LS NOW
            if(strlen(in_buff) == 3) {
                char dirs[LS_RESP_SIZE];
                memset(dirs, '\0', sizeof(dirs));
                recv_msg(state.sock_fd, dirs);
                printf("%s", dirs);

                return 0;
            }else{
                printf("%s", in_buff+3);
            }
        }
        else{
            //recieved nok sign - print to stderr
            char error[MAX_LEN];
            memset(error, '\0', sizeof(error));
            strcpy(error, in_buff+3);

            fprintf(stderr,  "Server Error: %s\n", error);
            FREE(state.client_id);
            FREE(state.server_addr);
            init_state();
            return 0; //disconnected, exit to client shell
        }
    }
    else if(strcmp(cmd->arguments[0], "get") == 0){
        return handle_get();
    }
    else{
        // UNKNOWN CMD -> send to receive nok
        if(send_msg(state.sock_fd, cmd->arguments[0]) == -1) {
            return -1;
        }
        if(recv_msg(state.sock_fd, in_buff) == -1){
            return -1;
        }
        if(strncmp("nok", in_buff, 3) == 0){

            fprintf(stderr,  "Server Error: %s\n", "unknown cmd");
            FREE(state.client_id);
            FREE(state.server_addr);
            init_state();
            return 0;
        }
    }
    return 0;
}

int main(int argc, char** args){
    if(argc > 1 && strcmp(args[1], "-d") == 0)
        debug = TRUE;

    init_state();

    while(1){
        printf("server: %s>", state.server_addr);

        fgets(buff, MAX_LEN, stdin);
        cmd = parse_cmd_lines(buff);
        refresh_buf();
        if(isQuit(cmd->arguments[0]))
            break;


        int ans = exec(cmd);
        switch (ans){
            case -1:
                exit(-1);
            case -2:
                exit(-2);
            default:
                break; // clean exec, continuing regularly
        }
        free_cmd_lines(cmd);
    }
    free_cmd_lines(cmd);
    return 0;
}