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
		
		switch (controlProtocol(user)){
			case 0:
				if (chatProtocol(user) == -1) should_exit = 1;
				break;
			case 1:
				if (queryProtocol(user) == -1) should_exit = 1;
				break;
			default :
				should_exit = 1;
				break;
		}
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
	
	//Socket
	int sock_fd, new_sock;

	//Program
	//Socket
	sock_fd = iniSocket();
	
	//User List
	iniClientDB();

	
	while(1){
		new_sock = accept(sock_fd, NULL, NULL);
		perror("accept");
		
		Client* user = createClient();
		user->sock = new_sock;
		
		pthread_create(&user->thread_id, NULL, server_thread, user);
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
int iniSocket(){
	int sock_fd;
	struct sockaddr_in addr, client_addr;
	int addr_len; 
	
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	perror("socket ");
	if(sock_fd == -1){
		exit(-1);
	}
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(3000);		/*Port*/
    addr.sin_addr.s_addr = INADDR_ANY;	/*IP*/	

	
	bind(sock_fd, (struct sockaddr *)  &addr, sizeof(addr));
	perror("bind");
	
	if( listen(sock_fd, 10) == -1){
		perror("listen ");
		exit(-1);
	}
	
	return sock_fd;
}
