#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "client-server.pb-c.h"
#include "coms.h"
#include "server-admin.h"

#define LOG_STR "LOG"
#define QUIT_STR "QUIT"
#define DISC_STR "DISC"

#define LOG 0
#define QUIT 1
#define DISC 2

#define STD_SIZE 100


//Function Declaration (.h not necessary)
int handleKeyboard();
int sendRequest(int action);
int receiveLog();
void iniSocket();

//Socket - Global Variablel
int sock_fd;

int main ( int arc, char **argv ) {

	//Socket
	iniSocket();
	
	while(1){
		if (handleKeyboard() != 0) break;
	}
	
	exit(0);
}

int handleKeyboard(){
	int should_exit = 0;
	char line[STD_SIZE];
	char command[STD_SIZE];
	
	fgets(line, STD_SIZE, stdin);
	if(sscanf(line, "%s", command) == 1){
		if(strcmp(command, LOG_STR) == 0){
			printf("Sending LOG request to server\n");
			if (sendRequest(LOG) == -1) should_exit = 1;
			if (receiveLog() == -1) should_exit = 1;
		}else if(strcmp(command, QUIT_STR)==0){
			printf("Sending QUIT request to server\n");
			sendRequest(QUIT);
			should_exit = 1;
		}else if(strcmp(command, DISC_STR)==0){
			printf("Sending DISC request to server\n");
			sendRequest(DISC);
			should_exit = 1;
		}
	}else{
		printf("Error in command\n");
	}
	return should_exit;
}

int sendRequest(int action){
	ADMIN message;
	admin__init(&message);
	message.action = action;
	proto_msg* disconnect_message = protoCreateAdmin(&message);

	return sendMessage( disconnect_message , sock_fd);
}

int receiveLog(){
	printf("Receiving LOG request from server\n");
	
	return 0;
}




/* iniSocket
 * 
 * Initializes the socket connection
 * Connects to server on PORT 3001, IP 127.0.0.1
 * 
 * @ returns sock_fd - Socket descriptor
 * */
void iniSocket(){
	struct sockaddr_in server_addr;
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd == -1){
		perror("socket ");
		exit(-1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(3001);				/*Port*/
	inet_aton("127.0.0.1", & server_addr.sin_addr); /*IP*/	

	if( connect(sock_fd, ( struct sockaddr *) &server_addr, sizeof(server_addr)) == -1){
		perror("connect ");
		exit(-1);
	}
	
	return;
}
