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
#include "client-msg-server.h"
#include "server.h"


//Message Processing
int loginProtocol(char *buffer){
	//Proto
	proto_msg * login_message = loginSendProto(buffer);
	
	//Send Message
	send(getSock(), login_message->msg, login_message->msg_size, 0);
	perror("send");
	destroyProtoMSG(login_message);

	//Read Message
	proto_msg * login_response_message = createProtoMSG();
	login_response_message->msg_size = read(getSock(), login_response_message->msg, BUFFER_SIZE);
	
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
	if (send(getSock(), msg, msg_size, 0) == -1){
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
	if (send(getSock(), msg, msg_size, 0) == -1){
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
			setLoginStatus(1);
			break;
		case 1:
			printf("Username in use\n");
			setLoginStatus(0);
			break;
		default:
			printf("Username invalid\n");
			setLoginStatus(0);
			break;
	}
	return;
}
