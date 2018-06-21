#define FREE(X) if(X) free((void*)X)
#define FALSE 0
#define TRUE 1
#define MAGIC_PORT 2018
#define MAX_LEN 2048
#define DIR_MAX_SIZE 2048
#define LS_RESP_SIZE 2048
#define FILE_BUFF_SIZE 1024
#define WRITE_EDIT_CREATE "w+"
#define READ "r"
#define ONESEC 1000

typedef enum {
	IDLE,
	CONNECTING,
	CONNECTED,
	DOWNLOADING,
	SIZE
} c_state;

typedef struct {
	char* server_addr;	// Address of the server as given in the [connect] command. "nil" if not connected to any server
	c_state conn_state;	// Current state of the client. Initially set to IDLE
	char* client_id;	// Client identification given by the server. NULL if not connected to a server.
	int sock_fd;
} client_state;

int file_size(char * filename);
char* list_dir();
int simple_cmp(char* str1, char* str2, int len);