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

#define LOGIN_STR "LOGIN"// username 
#define DISC_STR "DISC" //Disconnects from server
#define QUIT_STR "QUIT" //Exits the application
#define CHAT_STR "CHAT"// string
#define QUERY_STR "QUERY"// id_min id_max – request o

//Socket
int sock_fd;
int iniSocket();

//Server
int login_status;

//User Interface
int handleKeyboard();

//Chat
int chatProtocol(char *buffer);

//Query
int queryProtocol(int first_message, int last_message);
proto_msg *querySendProto(int first_message, int last_message);
void queryReceiveProto(proto_msg *query_response_message);

//Login
int loginProtocol(char *buffer);
proto_msg *loginSendProto(char *buffer);
void loginReceiveProto(proto_msg *login_response_message);

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
			printf("Receiving Broadcast from server...\n");
		}
	}
	exit(0);
}

/* handleKeyboard
 * 
 * Handles the client interface. Skeleton provided by professor
 * 
 * @ returns int - (1) for exit or (0) for repeat.
 * */
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
				discProtocol();
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

//DISC
/* discProtocol
 * Advises server of disconnection.
 * 
 * @ returns int - (-1) on error or (0) on success.
 * */
int discProtocol(){
	//Prepare message
	MESSAGE control;
	message__init(&control);
	control.next_message = 2;
	
	//Marshal message
	proto_msg* chat_message = protoCreateMessage(&control);
	
	//Send message
	if (sendMessage(chat_message, sock_fd) == -1) return -1;
	
	return 0;
}

//CHAT
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
	proto_msg* chat_message = protoCreateMessage(&control);
	
	free(chat.message);
	
	//Send message
	if (sendMessage(chat_message, sock_fd) == -1) return -1;
	
	return 0;
}

//QUERY
/* queryProtocol
 * 
 * Sends a message with desired first and last message and receives the server response.
 * 
 * @ first_message - id number
 * @ last_message - id number
 * 
 * @ returns int - 0 for success, -1 for error. 
 * */
int queryProtocol(int first_message, int last_message){
	
	proto_msg * query_message = querySendProto(first_message, last_message);
	
	if (sendMessage(query_message, sock_fd) == -1) return -1;
	
	proto_msg * query_response_message = receiveMessage(sock_fd);
	if (query_response_message == NULL) return -1;
	
	queryReceiveProto(query_response_message);
					
	return 0;
}

/* querySendProto
 * 
 * Prepares the message regarding QUERY
 * Does the marshal
 * 
 * @ first_message - id number
 * @ last_message - id number
 * 
 * @ returns proto_message - Marshaled message for the server
 * */
proto_msg *querySendProto(int first_message, int last_message){
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
	proto_msg* message = protoCreateMessage(&control);
	
	return message;
}

/* queryReceiveProto
 * 
 * Deals with Query Message Response from server.
 * Does the unmarshal and processes the response.
 * 
 * @ query_response_message - Structure with the login response from the server.
 * */
void queryReceiveProto(proto_msg *query_response_message){
	
	destroyProtoMSG(query_response_message);
	return;
}

//LOGIN
/* loginProtocol
 * 
 * Sends a message with desired username and receives the server response.
 * 
 * @ buffer - Buffer with the user name
 * @ returns int - 0 for success, -1 for error. 
 * */
int loginProtocol(char *buffer){
	//Proto
	proto_msg * login_message = loginSendProto(buffer);
	
	//Send Message
	if (sendMessage(login_message, sock_fd) == -1) return -1;

	//Read Message
	proto_msg * login_response_message = receiveMessage(sock_fd);
	if (login_response_message == NULL) return -1;
	
	//Proto
	loginReceiveProto(login_response_message);
					
	return 0;
}


/* loginSendProto
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
	
	free(login.username);
	
	return proto_message;
}

/* loginReceiveProto
 * 
 * Deals with Login Message Response from server.
 * Does the unmarshal and processes the response. Changes the login_status global variable.
 * 
 * @ login_response_message - Structure with the login response from the server.
 * */
void loginReceiveProto(proto_msg *login_response_message){	
	LOGIN *login_response = login__unpack(NULL, login_response_message->msg_size, login_response_message->msg);
	
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
	fflush(stdout);
	
	//Free involved memory
	destroyProtoMSG(login_response_message);
	login__free_unpacked(login_response, NULL);
	
	return;
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
