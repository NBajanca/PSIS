#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "client-server.pb-c.h"

#include "client-server.h"


proto_msg * receiveMessage(int sock){
	//Receive Message
	proto_msg * message = createProtoMSG();
	message->msg_size = read(sock, message->msg, BUFFER_SIZE);
	
	//Socket closed by client
	if ( message->msg_size == 0){
		printf("Socket Closed by Client\n");
		destroyProtoMSG(message);
		return NULL;
	}
	
	return message;
}


/* createProtoMSG
 * 
 * Creates Protocol Buffer Structure (MALLOC)
 * 
 * @ returns proto_message
 * */
proto_msg* createProtoMSG(){
	proto_msg* proto_message = (proto_msg*) malloc(sizeof(proto_msg));
	if( proto_message == NULL){
		perror("proto_message malloc ");
		exit(-1);
	}
	proto_message->msg = (char *) malloc (BUFFER_SIZE*sizeof(char));
	
	return proto_message;
}

/* destroyProtoMSG
 * 
 * Frees Protocol Buffer Structure (MALLOC)
 * 
 * @ proto_message
 * */
void destroyProtoMSG(proto_msg* proto_message){
	free(proto_message->msg);
	free(proto_message);
	
	return;
}

/* protoCreateLogin
 * 
 * Does the marshal of LOGIN Protocol Buffer 
 * 
 * @ login - Protocol buffer to marshal
 * @ returns proto_message
 * */
proto_msg* protoCreateLogin(LOGIN * login){
	proto_msg * proto_message = createProtoMSG();
	free(proto_message->msg);
	
	proto_message->msg_size = login__get_packed_size(login);
	proto_message->msg = malloc(proto_message->msg_size);
	login__pack(login, proto_message->msg);
	
	return proto_message;
}

/* protoCreateControl
 * 
 * Does the marshal of CONTROL Protocol Buffer 
 * 
 * @ control_msg - Protocol buffer to marshal
 * @ returns proto_message
 * */
proto_msg* protoCreateControl(CONTROL * control_msg){
	proto_msg * proto_message = createProtoMSG();
	free(proto_message->msg);
	
	proto_message->msg_size = control__get_packed_size(control_msg);
	proto_message->msg = malloc(proto_message->msg_size);
	control__pack(control_msg, proto_message->msg);
	
	return proto_message;
}

