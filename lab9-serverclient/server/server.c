#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/sendfile.h>
#include "common.h"

#define NOK_STATE  write(state.sock_fd, "nok state", strlen("nok state"));

client_state state;
char buff[MAX_LEN];
int debug = FALSE;

void refresh_buf(){
    int i=0;
    do{
        buff[i] = 0;
        ++i;
    }while(buff[i] != 0);
}
void init_state(){
    state.server_addr = malloc(MAX_LEN);
    gethostname(state.server_addr, MAX_LEN);
    state.conn_state = IDLE;
    state.client_id = NULL;
    state.sock_fd = -1;
}
void dc_client(){
    close(state.sock_fd);
    printf("Server @ %s dc'ed\n", state.client_id);

    free(state.client_id);
    init_state();
}
void debug_print(char* msg){
    if(debug){
        if(state.sock_fd == -1) {
            fprintf(stderr, "%s|Log %s\n", state.server_addr, "No client connected");
        }else{
            fprintf(stderr, "%s|Log %s\n", state.server_addr, msg);
        }
    }
}

int handle_conn(int client_fd){
    if(state.conn_state == IDLE){
        state.client_id = malloc(20);
        sprintf(state.client_id, "%d", client_fd);
        state.conn_state = CONNECTED;
        state.sock_fd = client_fd;

        sprintf(buff, "hello %s", state.client_id);
        write(state.sock_fd, buff, strlen(buff));
        //for testing
        printf("Client %s connected\n", state.client_id);
        return 0;
    }
    NOK_STATE
    return -1;
}
int handle_bye(){
    if(state.conn_state == CONNECTED){
        sprintf(buff, "bye");
        write(state.sock_fd, buff, strlen(buff));
        dc_client();
        return -5;
    }
    NOK_STATE
    return -1;
}
int handle_ls(){
    if(state.conn_state == CONNECTED){
        refresh_buf();
        char* dir_list = list_dir();
        if (!dir_list){
            sprintf(buff, "nok filesystem");
            write(state.sock_fd, buff, strlen("nok filesystem"));
            dc_client();
            printf("dir_list failed, client disconnected\n");
            return -5;
        }
        sprintf(buff, "ok ");
        write(state.sock_fd, buff, 3);
        refresh_buf();
        size_t len = strlen(dir_list);
        write(state.sock_fd, dir_list, len);
        printf("%s", dir_list);
        printf("Listed files at %s\n", getcwd(buff, MAX_LEN));
        refresh_buf();
        return 0;
    }
    NOK_STATE
    return -1;
}
int handle_get(){
    if(state.conn_state == CONNECTED){
        char filename[MAX_LEN], filename_cpy[MAX_LEN], filebuf[FILE_BUFF_SIZE];
        int filesize, wait_cycle = 10, itr;

        memset(filename, '\0', MAX_LEN);
        memset(filename_cpy, '\0', MAX_LEN);
        memset(filebuf, '\0', FILE_BUFF_SIZE);
        strcpy(filename, buff+4);
        strcpy(filename_cpy, filename);

        FILE* file;
        if((file = fopen(filename, READ)) < 0){
            write(state.sock_fd, "nok file", strlen("nok file"));
            dc_client();
            return -1;
        }

        filesize = file_size(filename);
        memset(filename, '\0', MAX_LEN);
        sprintf(filename, "ok %d", filesize);

        if(write(state.sock_fd, filename, strlen(filename)) < 0){ // sending "ok <SIZE>"
            printf("Server @ sending \"ok <SIZE>\" failed!");
            fclose(file);
            write(state.sock_fd, "nok", strlen("nok"));
            dc_client();
            return -1;
        }

        state.conn_state = DOWNLOADING;
        /** SENDFILE ATTEMPT */
        if(sendfile(state.sock_fd, file->_fileno, 0, FILE_BUFF_SIZE) < 0){
            printf("Server @ File sending failed!");
            fclose(file);
            write(state.sock_fd, "nok file", strlen("nok file"));
            dc_client();
            return -1;
        }

        // Loop read write in chunks!
        /*
        memset(filebuf, '\0', FILE_BUFF_SIZE);
        while(fread(filebuf, 1, FILE_BUFF_SIZE, file)){
            write(state.sock_fd, filebuf, strlen(filebuf));
            memset(filebuf, '\0', FILE_BUFF_SIZE);
        }
        */
        memset(filename, '\0', MAX_LEN);
        //wait for response "done" meaning file received
        while(wait_cycle > 0){
            if(recv(state.sock_fd, filename, MAX_LEN, 0) < 0){
                perror("Server @ Read failed\n");
                return -1;
            }
            if(strncmp(filename, "done", 4) == 0) {
                state.conn_state = CONNECTED;
                if (write(state.sock_fd, "ok", 2) < 0) {
                    perror("Server @ Couldn't send OK confirmation\n");
                    fclose(file);
                    return -1;
                }
                printf("Sent file %s\n", filename_cpy);
                fclose(file);
                return 0;
            }

            wait_cycle--;
            sleep(ONESEC);
        }
        if(write(state.sock_fd, "nok done", strlen("nok done")) < 0){
            dc_client();
            fclose(file);
            return -1;
        }
        memset(filename, '\0', MAX_LEN);
        strcpy(filename, "Server Error: ");
        strcat(filename, "nok, no done received");
        perror(filename);
        dc_client();
        fclose(file);
        return -1;


    }else{
        NOK_STATE
        return -1;
    }

    return 0;
}


int handle(int client_fd){
    debug_print(buff);

    if(strncmp(buff, "conn", 4) == 0)
        return handle_conn(client_fd);
    else if(strncmp(buff, "bye", 3) == 0)
        return handle_bye();
    else if(strncmp(buff, "ls", 2) == 0)
        return handle_ls();
    else if(strncmp(buff, "get", 3) == 0)
        return handle_get();
    else{
        fprintf(stderr, "%s|ERROR: Unknown message %s\n", state.client_id, buff);
        write(client_fd, "nok unknown cmd", strlen("nok unknown cmd"));
        dc_client();
        return -5;
    }
}


int accept_conn(){
    int server_fd, client_fd, ans;
    ssize_t r;
    int size_sockaddrin = sizeof(struct sockaddr_in);
    struct sockaddr_in serv, clnt;
    refresh_buf();

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        printf("Server @ socket creation failed\n");
        return -1;
    }
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(MAGIC_PORT);

    if(bind(server_fd,(struct sockaddr *) &serv , sizeof(serv)) < 0) {
        perror("Server @ Socket binding failed\n");
        return -1;
    }
    printf("Listening to socket\n");
    listen(server_fd, 1); //Await connection
    while(1) {
        if ((client_fd = accept(server_fd, (struct sockaddr *) &clnt, (socklen_t *) &size_sockaddrin)) < 0) {
            perror("Server @ Accepting client failed\n");
            return -1;
        }
        while ((r = recv(client_fd, buff, MAX_LEN, 0)) > 0) {
            ans = handle(client_fd);
            if (ans == -5) { //clean DC
                break;
            }else if(ans == -1){
                perror("Server @ handlers failed\n");
                break;
            }
            memset(buff, '\0', sizeof(buff));
        }
        if (r == -1 && ans != -5) {
            perror("Server @ Read failed\n");
            return -1;
        }
    }
    return 0;

}

int main(int argc, char** args){
    if(argc > 1 && strcmp(args[1], "-d") == 0)
        debug = TRUE;
    //while(1) {
    init_state();
    accept_conn();
    //}
    return 0;
}