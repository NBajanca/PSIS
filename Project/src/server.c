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

#include "client-server.h"
#include "server.h"

ClientDB *client_db;

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

		//Read Message
		proto_msg * login_message = createProtoMSG();
		login_message->msg_size = read(new_sock, login_message->msg, BUFFER_SIZE);
		
		//Proto
		proto_msg * proto_message = loginProtocol(login_message);
		destroyProtoMSG(login_message);
		
		//Send Message
		send(new_sock, proto_message->msg, proto_message->msg_size, 0);
		perror("send");
		destroyProtoMSG(proto_message);
			
		//Socket
		close(new_sock);
	}
	
	free(proto_message);
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
proto_msg *loginProtocol(proto_msg * login_message){
	//Unmarshal incoming message
	LOGIN *login = login__unpack(NULL, login_message->msg_size, login_message->msg);
	printf("New user: %s", login->username);
	
	//Prepare response message
	LOGIN login_response;
	login__init(&login_response);
	login_response.username = strdup(login->username);
	login_response.validation = 0;
	login_response.has_validation = 1;
	
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


/* iniClientDB
 * 
 * Initializes the Client List
 * Changes the global variable
 * 
 * */
void iniClientDB(){
	client_db = (ClientDB*) malloc(sizeof(ClientDB));
}
