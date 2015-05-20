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

void * server_thread(void *arg){
	Client* user = (Client*) arg;
	
	//Login Process
	do{
		login(user);
	}while (user->user_name == NULL);

	
	//Close Connection
	close(user->sock);
	removeClient(user);
	pthread_exit(NULL);
}

void login(Client* user){
	//Read Message
	proto_msg * login_message = createProtoMSG();
	login_message->msg_size = read(user->sock, login_message->msg, BUFFER_SIZE);
	
	//Login Protocol - PROTO and CLIENTDB
	proto_msg * proto_message = loginProtocol(login_message, user);
	destroyProtoMSG(login_message);
	
	//Send Message
	send(user->sock, proto_message->msg, proto_message->msg_size, 0);
	destroyProtoMSG(proto_message);
	
	return;
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

/* loginProtocol
 * 
 * Deals with Login Messages
 * Does the unmarshal and the user name verification
 * Prepares the response for the client
 * 
 * @ login_message - Structure with the login message from the client
 * @ returns proto_message - Marshaled message for the client
 * */
proto_msg *loginProtocol(proto_msg * login_message, Client* user){
	//Unmarshal incoming message
	LOGIN *login = login__unpack(NULL, login_message->msg_size, login_message->msg);
	
	//Prepare response message
	LOGIN login_response;
	login__init(&login_response);
	login_response.username = strdup(login->username);
	login_response.has_validation = 1;
	
	user->user_name = strdup(login->username);
	if (addClient(user) != -1){
		login_response.validation = 0;
	}else{
		login_response.validation = 1;
		user->user_name = NULL;
	}
	
	//Marshal response message
	proto_msg * proto_message = protoCreateLogin(&login_response);
	
	return proto_message;
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
