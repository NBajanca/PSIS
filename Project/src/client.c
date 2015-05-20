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
	char buffer[100];
	
	//Socket
	int sock_fd;
	
	//Proto
	proto_msg *login_message, *login_response_message;
	
	login_response_message = (proto_msg*) malloc(sizeof(proto_msg));
	if( login_response_message == NULL){
		perror("proto_message ");
		exit(-1);
	}
	login_response_message->msg = (char *) malloc (100*sizeof(char));

	//Program
	//Socket
	sock_fd = iniSocket();
	
	//User Interface
	printf("Username: ");
    fgets(buffer, 100, stdin);
	
	//Login Proto
	login_message = loginSendProtocol(buffer);
	
	//Send Message
	send(sock_fd, login_message->msg, login_message->msg_size, 0);
	perror("send");

	//Read Message
	login_response_message->msg_size = read(sock_fd, login_response_message->msg, 100);
	
	//Proto
	loginReceiveProtocol(login_response_message);
					
	exit(0);
	
}

proto_msg *loginSendProtocol(char *buffer){
	proto_msg *proto_message;
	
	LOGIN login;
	login__init(&login);
	char *login_msg;
	size_t login_size;
	
	proto_message = (proto_msg*) malloc(sizeof(proto_msg));
	if( proto_message == NULL){
		perror("proto_message ");
		exit(-1);
	}
	
	login.username = strdup(buffer);
	
	login_size = login__get_packed_size(&login);
    login_msg = malloc(login_size);
	login__pack(&login, login_msg);
	
	proto_message->msg = login_msg;
	proto_message->msg_size = login_size;
	
	return proto_message;
}

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
