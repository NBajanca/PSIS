#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <time.h>

#include "client-server.pb-c.h"

#include "client-server.h"
#include "client-db.h"
#include "server.h"
#include "server-msg-client.h"


//Message Processing
int loginProtocol(Client* user){
	//Receive Message
	proto_msg * login_message = receiveMessage(user->sock);
	if (login_message == NULL) return -1;
	
	//Login Protocol - PROTO and CLIENTDB
	proto_msg * proto_message = loginProto(login_message, user);
	destroyProtoMSG(login_message);
	
	//Send Message
	send(user->sock, proto_message->msg, proto_message->msg_size, 0);
	destroyProtoMSG(proto_message);
	
	return 0;
}

int controlProtocol(Client* user){
	//Receive Message
	proto_msg * control_message = receiveMessage(user->sock);
	if (control_message == NULL) return -1;
	
	//Unmarshal incoming message
	MESSAGE *control = message__unpack(NULL, control_message->msg_size, control_message->msg);
	destroyProtoMSG(control_message);
	fflush(stdout);
	
	switch (control->next_message){
		case 0:
			if (chatProtocol(user,control->chat) == -1) return -1;
			break;
		case 1:
			if (queryProtocol(user, control->query) == -1)return -1;
			break;
		case 2:
			return 1;
			break;
	}
	
	return ;
}

int chatProtocol(Client* user, CHAT *chat){
	char *time = getTime();
	
	printf("(%s) - %s : %s\n", time , user->user_name, chat->message);
	fflush(stdout);
	free(time);
	
	return 0;
}

int queryProtocol(Client* user, QUERY *query){

	return 0;
}


//LOGIN

/* loginProto
 * 
 * Deals with Login Messages
 * Does the unmarshal and the user name verification
 * Prepares the response for the client
 * 
 * @ login_message - Structure with the login message from the client
 * @ returns proto_message - Marshaled message for the client
 * */
proto_msg *loginProto(proto_msg * login_message, Client* user){
	//Unmarshal incoming message
	LOGIN *login = login__unpack(NULL, login_message->msg_size, login_message->msg);
	
	//Prepare response message
	LOGIN login_response;
	login__init(&login_response);
	login_response.username = strdup(login->username);
	login_response.has_validation = 1;
	
	user->user_name = strdup(login->username);
	if (addClient(user) != -1){
		login_response.validation = 0;
	}else{
		login_response.validation = 1;
		user->user_name = NULL;
	}
	
	//Marshal response message
	proto_msg * proto_message = protoCreateLogin(&login_response);
	
	return proto_message;
}


