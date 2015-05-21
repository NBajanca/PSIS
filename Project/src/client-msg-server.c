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

int chatProtocol(char *buffer){
	controlProtocol(0);

					
	return 0;
}

int queryProtocol(int first_message, int last_message){
	controlProtocol(1);
					
	return 0;
}

//CONTROL
int controlProtocol(int operation_type){
	//Prepare message
	CONTROL control;
	control__init(&control);
	control.next_message = operation_type;
	
	//Marshal message
	size_t msg_size = control__get_packed_size(&control);
	char *msg= malloc(msg_size);
	control__pack(&control, msg);
	
	//Send message
	send(getSock(), msg, msg_size, 0);
	perror("send");
	
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
