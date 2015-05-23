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
#include "client-server.h"
#include "client.h"

#define LOGIN_STR "LOGIN"// username 
#define DISC_STR "DISC" //Disconnects from server
#define QUIT_STR "QUIT" //Exits the application
#define CHAT_STR "CHAT"// string
#define QUERY_STR "QUERY"// id_min id_max – request o

//Socket
int sock_fd;

//Server
int login_status;

int main(){
	//Variables
	//General
	int should_exit = 0;
	
	//Program
	//Socket
	sock_fd = iniSocket();
	login_status = 0;
	
	//Select
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(STDIN_FILENO, &readfds);
	FD_SET(sock_fd, &readfds);
	
	while(! should_exit){
		select(sock_fd + 1, &readfds, NULL, NULL, NULL);
		if (FD_ISSET(STDIN_FILENO, &readfds)) {
			should_exit = handleKeyboard(); 
			
		}else if (FD_ISSET(sock_fd, &readfds)) {
			
		}
	}
	exit(0);
}

int handleKeyboard(){
	int should_exit = 0;
	char line[STD_SIZE];
	char command[STD_SIZE];
	char cmd_str_arg[STD_SIZE];
	int  cmd_int_arg1, cmd_int_arg2;
	
	fgets(line, STD_SIZE, stdin);
	if(sscanf(line, "%s", command) == 1){
		if(strcmp(command, LOGIN_STR) == 0 && !(login_status)){
			if(sscanf(line, "%*s %s", cmd_str_arg) == 1){
				printf("Sending LOGIN command (%s)\n", cmd_str_arg);
				if (loginProtocol(cmd_str_arg) == -1){
					close(sock_fd);
					should_exit= 1;	
				}
			}
			else{
				printf("Invalid LOGIN command\n");
			}
			
		}else if(strcmp(command, QUIT_STR)==0){
			printf("Exiting the app\n");
			close(sock_fd);
			should_exit= 1;						
		}else if (login_status){
			if(strcmp(command, DISC_STR)==0){
				printf("Sending DISconnnect command\n");
				close(sock_fd);
				login_status= 0;						
					
					
			}else if(strcmp(command, CHAT_STR)==0){
				if(sscanf(line, "%*s %s", cmd_str_arg) == 1){
					printf("Sending CHAT command (%s)\n", cmd_str_arg);
					if (chatProtocol(cmd_str_arg) == -1){
					close(sock_fd);
					should_exit= 1;	
					}
				
				}
				else{
					printf("Invalid CHAT command\n");
				}
			}else if(strcmp(command, QUERY_STR)==0){
				if(sscanf(line, "%*s %d %d", &cmd_int_arg1, &cmd_int_arg2) == 2){
					printf("Sending QUERY command (%d %d)\n", cmd_int_arg1, cmd_int_arg2);
					if (queryProtocol(cmd_int_arg1, cmd_int_arg2) == -1){
					close(sock_fd);
					should_exit= 1;	
					}
				
				}
				else{
					printf("Invalid QUERY command\n");
				}
			}else{
				printf("Invalid command\n");
			}
		}else{
				printf("Invalid command - LOGIN Required!\n");
		}
	}else{
		printf("Error in command\n");
	}
	return should_exit;
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
	if(sock_fd == -1){
		perror("socket ");
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

//Message Processing
int loginProtocol(char *buffer){
	//Proto
	proto_msg * login_message = loginSendProto(buffer);
	
	//Send Message
	send(sock_fd, login_message->msg, login_message->msg_size, 0);
	perror("send");
	destroyProtoMSG(login_message);

	//Read Message
	proto_msg * login_response_message = createProtoMSG(ALLOC_MSG);
	login_response_message->msg_size = read(sock_fd, login_response_message->msg, BUFFER_SIZE);
	
	//Proto
	loginReceiveProto(login_response_message);
	destroyProtoMSG(login_response_message);
					
	return 0;
}

/*chatProtocol
 * Receives a chat message from client and sends it to the server.
 * 
 * Description:
 * First advises the server that the incoming message is a chat and then sends the chat.
 * 
 * @ buffer - string containing the message to send.
 * @ returns int - (-1) on error or (0) on success.
 * */
int chatProtocol(char *buffer){
	//Prepare message
	MESSAGE control;
	message__init(&control);
	control.next_message = 0;
	
	CHAT chat;
	chat__init(&chat);
	chat.message = strdup(buffer);
	
	//CHAT chat_message = chatSendProto(buffer);
	control.chat = &chat;
	
	//Marshal message
	size_t msg_size = message__get_packed_size(&control);
	char *msg= malloc(msg_size);
	message__pack(&control, msg);
	
	//Send message
	if (send(sock_fd, msg, msg_size, 0) == -1){
		perror("Send (control)");
		return -1;
	}
	
	return 0;
}


int queryProtocol(int first_message, int last_message){
	//Prepare message
	MESSAGE control;
	message__init(&control);
	control.next_message = 1;
	
	QUERY query;
	query__init(&query);
	query.id_min = first_message;
	query.id_max = last_message;
	
	control.query = &query;
	
	//Marshal message
	size_t msg_size = message__get_packed_size(&control);
	char *msg= malloc(msg_size);
	message__pack(&control, msg);
	
	//Send message
	if (send(sock_fd, msg, msg_size, 0) == -1){
		perror("Send (control)");
		return -1;
	}
					
	return 0;
}


//LOGIN
/* loginSendProtocol
 * 
 * Prepares the message regarding LOGIN
 * Does the marshal
 * 
 * @ buffer - Buffer with the user name
 * @ returns proto_message - Marshaled message for the server
 * */
proto_msg *loginSendProto(char *buffer){
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
void loginReceiveProto(proto_msg *login_response_message){	
	LOGIN *login_response;
	
	login_response = login__unpack(NULL, login_response_message->msg_size, login_response_message->msg);
	
	switch ( login_response->validation ) {
		case 0:
			printf("User Login with success\n");
			login_status = 1;
			break;
		case 1:
			printf("Username in use\n");
			login_status = 0;
			break;
		default:
			printf("Username invalid\n");
			login_status = 0;
			break;
	}
	return;
}
