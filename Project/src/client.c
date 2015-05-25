#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <arpa/inet.h>

#include "client-server.pb-c.h"
#include "coms.h"
#include "client-server.h"

#define LOGIN_STR "LOGIN"// username 
#define DISC_STR "DISC"
#define CHAT_STR "CHAT"// string
#define QUERY_STR "QUERY"// id_min id_max

//Socket
int sock_fd;
void iniSocket();

//Server
int login_status;
int serverMessages();
int discProtocol();

//User Interface
int handleKeyboard();

//Chat
int chatProtocol(char *buffer);

//Query
int queryProtocol(int first_message, int last_message);
proto_msg *querySendProto(int first_message, int last_message);
void queryReceiveProtocol(QUERY *query_message);

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
	iniSocket();
	login_status = 0;
	
	//Select
	fd_set readfds;
	
	
	while(! should_exit){
		FD_ZERO(&readfds);
		FD_SET(0, &readfds);
		FD_SET(sock_fd, &readfds);
	
		if (select(sock_fd + 1, &readfds, NULL, NULL, NULL) == -1){
			perror("[System Error] Select ");
			exit(-1);
		}
		
		if (FD_ISSET(sock_fd, &readfds)) {
			should_exit = serverMessages();
		}
		
		if (FD_ISSET(0, &readfds)) {
			should_exit = handleKeyboard(); 
		}
	}
	
	close(sock_fd);
	printf("\nSee you soon!\n");
	exit(0);
}

/* serverMessages
 * 
 * Handles the server messages.
 * 
 * @ returns int - (1) for exit or (0) for repeat.
 * */
int serverMessages(){
	proto_msg *broadcast_message;
	MESSAGE * broadcat_message_aux;
	
	broadcast_message = receiveMessage(sock_fd);
	if (broadcast_message == NULL){
		printf("[System] Error receiving messages from server. Disconnecting...\n");
		return -1;
	}
	broadcat_message_aux = message__unpack(NULL, broadcast_message->msg_size, (const uint8_t *)broadcast_message->msg);
	if (broadcat_message_aux == NULL){
		printf("[System] Message from server in incorrect format (Discarted)\n");
		return 0;
	}
	
	switch (broadcat_message_aux->next_message){
		case 0:
			printf("\t%s\n", broadcat_message_aux->chat->message);
			break;
		case 1:
			queryReceiveProtocol(broadcat_message_aux->query);
			break;
		case 2:
			printf("\tServer is Shuting Down\n");
			message__free_unpacked(broadcat_message_aux, NULL);
			destroyProtoMSG(broadcast_message);
			return 1;
	}
	
	message__free_unpacked(broadcat_message_aux, NULL);
	destroyProtoMSG(broadcast_message);
	
	return 0;
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
			if((sscanf(line, "%*s %s", cmd_str_arg) == 1) && (strlen (cmd_str_arg) <= USERNAME_SIZE_LIMIT)){
				printf("[User] Sending LOGIN command (%s)\n", cmd_str_arg);
				if (loginProtocol(cmd_str_arg) == -1){
					close(sock_fd);
					should_exit= 1;	
				}
			}
			else{
				printf("[User] Invalid LOGIN command\n");
			}
			
		}else if(strcmp(command, DISC_STR)==0 && !(login_status)){
			should_exit= 1;						
		}else if (login_status){
			if(strcmp(command, DISC_STR)==0){
				printf("[User] Sending DISconnnect command\n");
				discProtocol();
				should_exit= 1;						

			}else if(strcmp(command, CHAT_STR)==0){
				if((sscanf(line, "%*s %s", cmd_str_arg) == 1) && (strlen (cmd_str_arg) <= CHATSTRING_SIZE_LIMIT)){
					printf("[User] Sending CHAT command (%s)\n", cmd_str_arg);
					if (chatProtocol(cmd_str_arg) == -1){
					should_exit= 1;	
					}
				
				}
				else{
					printf("[User] Invalid CHAT command\n");
				}
			}else if(strcmp(command, QUERY_STR)==0){
				if((sscanf(line, "%*s %d %d", &cmd_int_arg1, &cmd_int_arg2) == 2) && (cmd_int_arg1 > 1) && (cmd_int_arg2 > cmd_int_arg1)){
					printf("[User] Sending QUERY command (%d %d)\n", cmd_int_arg1, cmd_int_arg2);
					if (queryProtocol(cmd_int_arg1, cmd_int_arg2) == -1){
					should_exit= 1;	
					}
				
				}
				else{
					printf("[User] Invalid QUERY command\n");
				}
			}else{
				printf("[User] Invalid command\n");
			}
		}else{
				printf("[User] Invalid command - LOGIN Required!\n");
		}
	}else{
		printf("[User] Error in command\n");
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
void queryReceiveProtocol(QUERY *query_message){
	int num_messages, i;
	
	num_messages = query_message->id_max - query_message->id_min +1;
	
	for (i = 0; i < num_messages; i++){
		printf("\t[%d] %s\n", query_message->messages[i]->id ,query_message->messages[i]->message);
	}
	
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
	LOGIN *login_response = login__unpack(NULL, login_response_message->msg_size, (const uint8_t *)login_response_message->msg);
	if (login_response == NULL){
		printf("[System] Message from server in incorrect format (Discarted)\n");
		return;
	}
	
	switch ( login_response->validation ) {
		case 0:
			printf("\tUser Login with success\n");
			login_status = 1;
			break;
		case 1:
			printf("\tUsername in use\n");
			login_status = 0;
			break;
		default:
			printf("\tUsername invalid\n");
			login_status = 0;
			break;
	}
	
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
void iniSocket(){
	struct sockaddr_in server_addr;
	
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd == -1){
		perror("[System Error] Socket ");
		exit(-1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(3000);				/*Port*/
	inet_aton("127.0.0.1", & server_addr.sin_addr); /*IP*/	

	if( connect(sock_fd, ( struct sockaddr *) &server_addr, sizeof(server_addr)) == -1){
		perror("[System Error] Connect ");
		exit(-1);
	}
	
	return;
}
