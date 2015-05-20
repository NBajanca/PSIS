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

int main(){
	//Variables
	//General
	char buffer[100];
	proto_msg *proto_message;
	
	//Socket
	int sock_fd, new_sock;
	
	//Proto
	proto_msg *login_message;
	login_message = (proto_msg*) malloc(sizeof(proto_msg));
	if( login_message == NULL){
		perror("proto_message ");
		exit(-1);
	}
	login_message->msg = (char *) malloc (100*sizeof(char));

	//Program
	//Socket
	sock_fd = iniSocket();

	
	while(1){
		new_sock = accept(sock_fd, NULL, NULL);
		perror("accept");

		//Read Message
		login_message->msg_size = read(new_sock, login_message->msg, 100);
		
		//Proto 
		proto_message = loginProtocol(login_message);
		
		//Send Message
		send(new_sock, proto_message->msg, proto_message->msg_size, 0);
		perror("send");
			
		//Socket
		close(new_sock);
	}
	
	free(proto_message);
	exit(0);
	
}

proto_msg *loginProtocol(proto_msg * login_message){
	proto_msg *proto_message;
	
	proto_message = (proto_msg*) malloc(sizeof(proto_msg));
	if( proto_message == NULL){
		perror("proto_message ");
		exit(-1);
	}
	
	LOGIN *login , login_response;
	login__init(&login_response);
	char *login_response_msg;
	size_t login_response_size;
	
	login = login__unpack(NULL, login_message->msg_size, login_message->msg);
	printf("New user: %s", login->username);
	
	login_response.username = strdup(login->username);
	login_response.validation = 0;
	login_response.has_validation = 1;
	
	proto_message->msg_size = login__get_packed_size(&login_response);
	proto_message->msg = malloc(proto_message->msg_size);
	login__pack(&login_response, proto_message->msg);
	
	return proto_message;
}


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
