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

int main(){
	//Variables
	//General
	char buffer[100];
	
	//Socket
	int sock_fd, new_sock;
	struct sockaddr_in addr, client_addr;
	int addr_len; 
	
	//Proto
	LOGIN *login , login_response;
	login__init(&login_response);
	char login_msg[100], *login_response_msg;
	size_t login_size, login_response_size;

	//Program
	//Socket
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
	
	while(1){
		new_sock = accept(sock_fd, NULL, NULL);
		perror("accept");

		//Read Message
		login_size = read(new_sock, login_msg, 100);
		
		//Proto 
		login = login__unpack(NULL, login_size, login_msg);
		printf("New user: %s", login->username);
		
		login_response.username = strdup(login->username);
		login_response.validation = 0;
		login_response.has_validation = 1;
		
		login_response_size = login__get_packed_size(&login_response);
		login_response_msg = malloc(login_response_size);
		login__pack(&login_response, login_response_msg);
		
		//Send Message
		send(new_sock, login_response_msg, login_response_size, 0);
		perror("send");
			
		//Socket
		close(new_sock);	
	}
	

	exit(0);
	
}
