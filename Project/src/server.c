#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <pthread.h>

#include "client-server.pb-c.h"

#include "client-server.h"
#include "client-db.h"
#include "server.h"
#include "server-msg-client.h"

#define LOG_STR "LOG"
#define QUIT_STR "QUIT"
#define max( a, b) ( ((a) > (b)) ? (a) : (b) )

void * server_thread(void *arg){
	Client* user = (Client*) arg;
	int should_exit = 0;
	
	//Login Process
	do{
		if (loginProtocol(user) == -1){
			should_exit = 1;
			break;
		}
	}while (user->user_name == NULL);
	
	//Comunication with Client
	while(! should_exit){
		if (controlProtocol(user) != 0) should_exit = 1;
	}
	
	//Close Connection
	close(user->sock);
	removeClient(user);
	pthread_exit(NULL);
}

int main(){
	//Variables
	//General
	char buffer[BUFFER_SIZE];
	proto_msg *proto_message;
	int should_exit = 0;
	
	//Socket
	int sock_fd, sock_admin_fd, new_sock, max_fd;

	//Program
	//Socket
	sock_fd = iniSocket(3000); //SERVER
	sock_admin_fd = iniSocket(3001); //ADMIN
	
	//User List
	iniClientDB();
	
	//Select
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(sock_admin_fd, &readfds);
	FD_SET(sock_fd, &readfds);
	max_fd = max(sock_fd, sock_admin_fd);
	
	char line[CMD_SIZE];
	char command[CMD_SIZE];
	while(! should_exit){
		select(max_fd + 1, &readfds, NULL, NULL, NULL);
		if (FD_ISSET(sock_admin_fd, &readfds)) {
			fgets(line, CMD_SIZE, stdin);
			if(sscanf(line, "%s", command) == 1){
				if(strcmp(command, LOG_STR) == 0){
					printf("LOG is being processed\n");
					//LOG print to implement
				}else if(strcmp(command, QUIT_STR)==0){
					printf("Shuting down the server\n");
					should_exit= 1;						
				}
			}		
		}else if (FD_ISSET(sock_fd, &readfds)) {
			new_sock = accept(sock_fd, NULL, NULL);
			if(sock_fd == -1){
				perror("Accept (Client) ");
				exit(-1);
			}
			Client* user = createClient();
			user->sock = new_sock;
			
			pthread_create(&user->thread_id, NULL, server_thread, user);
		}
	}
	
	destroyClientDB();
	exit(0);
}

/* iniSocket
 * 
 * Initializes the socket connection
 * Accepts connections on PORT 3000, from any IP
 * 
 * Binds and then listen with a maximum 10 connections
 * 
 * @ returns sock_fd - Socket descriptor
 * */
int iniSocket(int port){
	int sock_fd;
	struct sockaddr_in addr, client_addr;
	int addr_len; 
	
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd == -1){
		perror("Socket ");
		exit(-1);
	}
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);		/*Port*/
    addr.sin_addr.s_addr = INADDR_ANY;	/*IP*/	

	
	if( bind(sock_fd, (struct sockaddr *)  &addr, sizeof(addr)) == -1){
		perror("Bind ");
		exit(-1);
	}
	
	if( listen(sock_fd, 10) == -1){
		perror("Listen ");
		
	}
	
	return sock_fd;
}

char * getTime(){
	char *time_string;
	time_string = (char *) malloc( CMD_SIZE * sizeof(char));

	time_t t = time(NULL);
	struct tm * p = localtime(&t);

	strftime(time_string, CMD_SIZE, "%F, %T", p);
	
	return time_string;
}
