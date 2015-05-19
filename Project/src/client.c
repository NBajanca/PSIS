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

#include "client.h"

int main(){
	//Variables
	//General
	char buffer[100];
	
	//Socket
	int sock_fd;
	
	//Proto
	LOGIN login , *login_response;
	login__init(&login);
	char *login_msg, login_response_msg[100];
	size_t login_size, login_response_size;

	//Program
	//Socket
	sock_fd = iniSocket();
	
	
	//User Interface
	printf("Username: ");
    fgets(buffer, 100, stdin);
    login.username = strdup(buffer);
    
    //Proto
    login_size = login__get_packed_size(&login);
    login_msg = malloc(login_size);
	login__pack(&login, login_msg);
	
	//Send Message
	send(sock_fd, login_msg, login_size, 0);
	perror("send");

	//Read Message
	login_response_size = read(sock_fd, &login_response_msg, 100);
	
	//Proto
	login_response = login__unpack(NULL, login_response_size, login_response_msg);
	printf("Response Code: %d\n", login_response->validation);
	
	switch ( login_response->validation ) {
		case 0:
			printf("User Login with success\n");
			break;
		case 1:
			printf("Username in use\n");
			exit(0);
		default:
			printf("Username invalid\n");
			exit(1);
	}
					
	exit(0);
	
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
