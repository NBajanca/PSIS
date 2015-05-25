#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "client-server.pb-c.h"
#include "coms.h"

#include "client-server.h"


/* protoCreateLogin
 * 
 * Does the marshal of LOGIN Protocol Buffer 
 * 
 * @ login - Protocol buffer to marshal
 * @ returns proto_message
 * */
proto_msg* protoCreateLogin(LOGIN * login){
	proto_msg * proto_message = createProtoMSG(DONT_ALLOC_MSG);
	
	proto_message->msg_size = login__get_packed_size(login);
	proto_message->msg = malloc(proto_message->msg_size);
	login__pack(login, (uint8_t *) proto_message->msg);
	
	return proto_message;
}

/* protoCreateControl
 * 
 * Does the marshal of CONTROL Protocol Buffer 
 * 
 * @ control_msg - Protocol buffer to marshal
 * @ returns proto_message
 * */
proto_msg* protoCreateMessage(MESSAGE * control_msg){
	proto_msg * proto_message = createProtoMSG(DONT_ALLOC_MSG);
	
	proto_message->msg_size = message__get_packed_size(control_msg);
	proto_message->msg = malloc(proto_message->msg_size);
	message__pack(control_msg, (uint8_t *) proto_message->msg);
	
	return proto_message;
}
