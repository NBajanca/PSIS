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
#include "client.h"


int main(){
	//Variables
	//General
	char buffer[BUFFER_SIZE];
	
	//Socket
	int sock_fd;

	//Program
	//Socket
	sock_fd = iniSocket();
	
	//User Interface
	printf("Username: ");
    fgets(buffer, BUFFER_SIZE, stdin);
	
	//Login Proto
	proto_msg * login_message = loginSendProtocol(buffer);
	
	//Send Message
	send(sock_fd, login_message->msg, login_message->msg_size, 0);
	perror("send");
	destroyProtoMSG(login_message);

	//Read Message
	proto_msg * login_response_message = createProtoMSG();
	login_response_message->msg_size = read(sock_fd, login_response_message->msg, BUFFER_SIZE);
	
	//Proto
	loginReceiveProtocol(login_response_message);
	destroyProtoMSG(login_response_message);
					
	exit(0);
	
}

/* loginSendProtocol
 * 
 * Prepares the message regarding LOGIN
 * Does the marshal
 * 
 * @ buffer - Buffer with the user name
 * @ returns proto_message - Marshaled message for the server
 * */
proto_msg *loginSendProtocol(char *buffer){
	//Prepare message
	LOGIN login;
	login__init(&login);
	login.username = strdup(buffer);
	
	//Marshal message
	proto_msg * proto_message = protoCreateLogin(&login);
	
	return proto_message;
}

/* loginReceiveProtocol
 * 
 * Deals with Login Message Response from server
 * Does the unmarshal
 * 
 * @ login_response_message - Structure with the login response from the server
 * @ returns int - Login information
 * */
int loginReceiveProtocol(proto_msg *login_response_message){	
	LOGIN *login_response;
	
	login_response = login__unpack(NULL, login_response_message->msg_size, login_response_message->msg);
	printf("Response Code: %d\n", login_response->validation);
	
	switch ( login_response->validation ) {
		case 0:
			printf("User Login with success\n");
			return 0;
		case 1:
			printf("Username in use\n");
			return 1;
		default:
			printf("Username invalid\n");
			return -1;
	}
	return 0;
}


/* iniSocket
 * 
 * Initializes the socket connection
 * Connects to server on PORT 3000, IP 127.0.0.1
 * 
 * @ returns sock_fd - Socket descriptor
 * */
int iniSocket(){
	int sock_fd;
	struct sockaddr_in server_addr;
	
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	perror("socket ");
	if(sock_fd == -1){
		exit(-1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(3000);				/*Port*/
	inet_aton("127.0.0.1", & server_addr.sin_addr); /*IP*/	

	if( connect(sock_fd, ( struct sockaddr *) &server_addr, sizeof(server_addr)) == -1){
		perror("connect ");
		exit(-1);
	}
	
	return sock_fd;
}
