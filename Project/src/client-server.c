#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "client-server.pb-c.h"

#include "client-server.h"

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
 * @ login_response - Protocol buffer to marshal
 * @ returns proto_message
 * */
proto_msg* protoCreateLogin(LOGIN * login_response){
	proto_msg * proto_message = createProtoMSG();
	free(proto_message->msg);
	
	proto_message->msg_size = login__get_packed_size(login_response);
	proto_message->msg = malloc(proto_message->msg_size);
	login__pack(login_response, proto_message->msg);
	
	return proto_message;
}


